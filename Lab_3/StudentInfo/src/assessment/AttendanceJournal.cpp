#include "studentis/assessment/AttendanceJournal.h"

#include "studentis/academic/Course.h"
#include "studentis/academic/Lesson.h"
#include "studentis/assessment/Attendance.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/exceptions/AttendanceException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "present" || status == "absent" || status == "excused" || status == "late";
}

bool countsAsAttended(const std::string& status) {
    return status == "present" || status == "late" || status == "excused";
}

double percent(int counted, int total) {
    return total == 0 ? 0.0 : 100.0 * static_cast<double>(counted) / static_cast<double>(total);
}

} // namespace

AttendanceJournal::AttendanceJournal(StudentGroup* group, Course* course)
    : group_(group), course_(course) {
    if (group_ == nullptr) throw InvalidStudentDataException("журнал должен относиться к группе");
    if (course_ == nullptr) throw InvalidStudentDataException("журнал должен относиться к курсу");
}

void AttendanceJournal::mark(Attendance& entry) {
    if (group_ == nullptr || course_ == nullptr) {
        throw AttendanceException("журнал не привязан к группе или курсу");
    }
    if (entry.getStudent() == nullptr || entry.getLesson() == nullptr) {
        throw AttendanceException("отметка не содержит студента или занятия");
    }
    if (!isKnownStatus(entry.getStatus())) {
        throw AttendanceException("неизвестный статус посещаемости: " + entry.getStatus());
    }
    if (group_->findStudent(entry.getStudent()->getStudentId()) != entry.getStudent()) {
        throw AttendanceException("студент не входит в группу журнала");
    }
    if (entry.getLesson()->getCourse() != course_) {
        throw AttendanceException("занятие относится к другому курсу");
    }
    for (Attendance*& current : entries_) {
        if (current->getStudent() == entry.getStudent() && current->getLesson() == entry.getLesson()) {
            current = &entry;
            return;
        }
    }
    entries_.push_back(&entry);
}

double AttendanceJournal::attendanceRate() const {
    int attended = 0;
    for (const Attendance* entry : entries_) {
        if (entry != nullptr && countsAsAttended(entry->getStatus())) ++attended;
    }
    return percent(attended, static_cast<int>(entries_.size()));
}

int AttendanceJournal::absentCount() const {
    int count = 0;
    for (const Attendance* entry : entries_) {
        if (entry != nullptr && entry->getStatus() == "absent") ++count;
    }
    return count;
}

int AttendanceJournal::unexcusedAbsences(const Student& student) const {
    int count = 0;
    for (const Attendance* entry : entries_) {
        if (entry != nullptr && entry->getStudent() != nullptr &&
            entry->getStudent()->getStudentId() == student.getStudentId() &&
            entry->getStatus() == "absent") {
            ++count;
        }
    }
    return count;
}

std::vector<Student*> AttendanceJournal::chronicAbsentees(int minAbsences) const {
    std::vector<Student*> result;
    if (group_ == nullptr || minAbsences <= 0) return result;
    for (Student* student : group_->getStudents()) {
        if (student != nullptr && unexcusedAbsences(*student) >= minAbsences) {
            result.push_back(student);
        }
    }
    return result;
}

double AttendanceJournal::lessonRate(const Lesson& lesson) const {
    int total = 0;
    int attended = 0;
    for (const Attendance* entry : entries_) {
        if (entry == nullptr || entry->getLesson() != &lesson) continue;
        ++total;
        if (countsAsAttended(entry->getStatus())) ++attended;
    }
    return percent(attended, total);
}

} // namespace studentis
