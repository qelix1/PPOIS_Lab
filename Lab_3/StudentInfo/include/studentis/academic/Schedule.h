#ifndef STUDENTIS_ACADEMIC_SCHEDULE_H
#define STUDENTIS_ACADEMIC_SCHEDULE_H

#include <string>
#include <vector>

namespace studentis {

class Lesson;
class Semester;
class StudentGroup;

/**
 * @brief Расписание группы на семестр: занятия, проверка конфликтов и публикация.
 */
class Schedule {
public:
    Schedule() = default;
    Schedule(std::string id, StudentGroup* group, Semester* semester, bool published = false);

    /// Добавить занятие; наложение для группы, преподавателя или аудитории — ScheduleConflictException.
    void addLesson(Lesson& lesson);
    /// Убрать занятие; true — если оно было в расписании.
    bool removeLesson(const Lesson& lesson);
    /// Занятия на указанную дату.
    std::vector<Lesson*> lessonsOn(const std::string& date) const;
    /// Занятия в указанный день недели (1 — понедельник, 7 — воскресенье).
    std::vector<Lesson*> lessonsForWeekday(int weekday) const;
    /// В расписании нет наложений занятий.
    bool validateNoConflicts() const;
    /// Опубликовать расписание; при конфликтах — ScheduleConflictException.
    void publish();
    /// Первое свободное время начала занятия не короче durationMinutes; нет места — конфликт.
    std::string findFreeSlot(const std::string& date, const std::string& preferredStart,
                             int durationMinutes) const;
    /// Суммарная недельная нагрузка в часах.
    int totalWeeklyHours() const;

    const std::string& getId() const { return id_; }
    StudentGroup* getGroup() const { return group_; }
    Semester* getSemester() const { return semester_; }
    const std::vector<Lesson*>& getLessons() const { return lessons_; }
    bool isPublished() const { return published_; }

    void setGroup(StudentGroup* group) { group_ = group; }
    void setSemester(Semester* semester) { semester_ = semester; }

private:
    std::string id_;
    StudentGroup* group_ = nullptr;
    Semester* semester_ = nullptr;
    std::vector<Lesson*> lessons_;
    bool published_ = false;
};

} // namespace studentis

#endif
