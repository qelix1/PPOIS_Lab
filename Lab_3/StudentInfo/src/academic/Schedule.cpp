#include "studentis/academic/Schedule.h"

#include <cstdio>

#include "studentis/academic/Course.h"
#include "studentis/academic/Lesson.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/ScheduleConflictException.h"

namespace studentis {
namespace {

int minutesOf(const std::string& time) {
    return date::minutesOfDateTime("2000-01-01 " + time);
}

std::string formatTime(int minutes) {
    char buffer[6];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes / 60, minutes % 60);
    return std::string(buffer);
}

// День недели ISO: 1 — понедельник, 7 — воскресенье (алгоритм Сакамото).
int isoWeekday(const std::string& date) {
    if (!date::isDate(date)) return 0;
    int year = 0;
    int month = 0;
    int day = 0;
    std::sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
    static const int offsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if (month < 3) --year;
    const int sundayBased =
        (year + year / 4 - year / 100 + year / 400 + offsets[month - 1] + day) % 7;
    return sundayBased == 0 ? 7 : sundayBased;
}

} // namespace

Schedule::Schedule(std::string id, StudentGroup* group, Semester* semester, bool published)
    : id_(std::move(id)), group_(group), semester_(semester), published_(published) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор расписания не может быть пустым");
}

void Schedule::addLesson(Lesson& lesson) {
    for (const Lesson* current : lessons_) {
        if (current == &lesson) return;
    }
    if (lesson.getCourse() != nullptr && lesson.getCourse()->getGroup() != nullptr && group_ != nullptr &&
        lesson.getCourse()->getGroup() != group_) {
        throw InvalidStudentDataException("занятие относится к другой группе");
    }
    auto groupOf = [this](const Lesson& value) -> const StudentGroup* {
        if (value.getCourse() != nullptr && value.getCourse()->getGroup() != nullptr) {
            return value.getCourse()->getGroup();
        }
        return group_;
    };
    auto teacherOf = [](const Lesson& value) -> const Teacher* {
        return value.getCourse() != nullptr ? value.getCourse()->getTeacher() : nullptr;
    };
    for (const Lesson* current : lessons_) {
        if (!lesson.overlaps(*current)) continue;
        const bool sameGroup = groupOf(lesson) == groupOf(*current);
        const Teacher* teacher = teacherOf(lesson);
        const bool sameTeacher = teacher != nullptr && teacher == teacherOf(*current);
        const bool sameClassroom =
            lesson.getClassroom() != nullptr && lesson.getClassroom() == current->getClassroom();
        if (sameGroup || sameTeacher || sameClassroom) {
            throw ScheduleConflictException("занятие " + lesson.getId() +
                                            " пересекается с занятием " + current->getId());
        }
    }
    lessons_.push_back(&lesson);
    published_ = false;
}

bool Schedule::removeLesson(const Lesson& lesson) {
    for (auto it = lessons_.begin(); it != lessons_.end(); ++it) {
        if (*it == &lesson) {
            lessons_.erase(it);
            published_ = false;
            return true;
        }
    }
    return false;
}

std::vector<Lesson*> Schedule::lessonsOn(const std::string& date) const {
    std::vector<Lesson*> result;
    for (Lesson* lesson : lessons_) {
        if (lesson->isOn(date)) result.push_back(lesson);
    }
    return result;
}

std::vector<Lesson*> Schedule::lessonsForWeekday(int weekday) const {
    std::vector<Lesson*> result;
    if (weekday < 1 || weekday > 7) return result;
    for (Lesson* lesson : lessons_) {
        if (isoWeekday(lesson->getDate()) == weekday) result.push_back(lesson);
    }
    return result;
}

bool Schedule::validateNoConflicts() const {
    for (std::size_t i = 0; i < lessons_.size(); ++i) {
        for (std::size_t j = i + 1; j < lessons_.size(); ++j) {
            if (lessons_[i]->overlaps(*lessons_[j])) return false;
        }
    }
    return true;
}

void Schedule::publish() {
    if (!validateNoConflicts()) {
        throw ScheduleConflictException("расписание " + id_ + " содержит наложения занятий");
    }
    published_ = true;
}

std::string Schedule::findFreeSlot(const std::string& date, const std::string& preferredStart,
                                   int durationMinutes) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата: " + date);
    if (durationMinutes <= 0) {
        throw InvalidStudentDataException("длительность занятия должна быть положительной");
    }
    int candidate = minutesOf(preferredStart);
    if (candidate < 0) throw InvalidStudentDataException("некорректное время начала: " + preferredStart);
    for (; candidate + durationMinutes <= 24 * 60; candidate += 15) {
        bool busy = false;
        for (const Lesson* lesson : lessons_) {
            if (lesson->getDate() != date) continue;
            const int start = minutesOf(lesson->getStartTime());
            const int end = minutesOf(lesson->getEndTime());
            if (candidate < end && start < candidate + durationMinutes) {
                busy = true;
                break;
            }
        }
        if (!busy) return formatTime(candidate);
    }
    throw ScheduleConflictException("в расписании " + id_ + " нет свободного окна на " + date);
}

int Schedule::totalWeeklyHours() const {
    int minutes = 0;
    for (const Lesson* lesson : lessons_) minutes += lesson->durationMinutes();
    return minutes / 60;
}

} // namespace studentis
