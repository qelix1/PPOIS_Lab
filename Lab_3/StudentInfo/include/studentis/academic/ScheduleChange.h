#ifndef STUDENTIS_ACADEMIC_SCHEDULECHANGE_H
#define STUDENTIS_ACADEMIC_SCHEDULECHANGE_H

#include <string>

namespace studentis {

class Classroom;
class Lesson;
class StudentGroup;
class Teacher;

/**
 * @brief Изменение расписания: перенос занятия с согласованием и применением.
 */
class ScheduleChange {
public:
    ScheduleChange() = default;
    ScheduleChange(std::string id, Lesson* lesson, std::string oldDate, std::string newDate,
                   Classroom* oldClassroom, Classroom* newClassroom, std::string reason,
                   Teacher* approvedBy, std::string createdAt);

    /// Согласовать изменение преподавателем.
    void approve(Teacher& teacher);
    /// Применить изменение к занятию; без согласования — ошибка данных.
    void apply();
    /// Отменить изменение и вернуть занятие на прежнее место.
    void cancel();
    /// Изменение уже применено к занятию.
    bool isApplied() const;
    /// Изменение затрагивает указанную группу.
    bool affectsGroup(const StudentGroup& group) const;
    /// Краткое текстовое описание изменения.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Lesson* getLesson() const { return lesson_; }
    const std::string& getOldDate() const { return oldDate_; }
    const std::string& getNewDate() const { return newDate_; }
    Classroom* getOldClassroom() const { return oldClassroom_; }
    Classroom* getNewClassroom() const { return newClassroom_; }
    const std::string& getReason() const { return reason_; }
    Teacher* getApprovedBy() const { return approvedBy_; }
    const std::string& getCreatedAt() const { return createdAt_; }

    void setReason(const std::string& reason) { reason_ = reason; }

private:
    std::string id_;
    Lesson* lesson_ = nullptr;
    std::string oldDate_;
    std::string newDate_;
    Classroom* oldClassroom_ = nullptr;
    Classroom* newClassroom_ = nullptr;
    std::string reason_;
    Teacher* approvedBy_ = nullptr;
    std::string createdAt_;
};

} // namespace studentis

#endif
