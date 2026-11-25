#ifndef STUDENTIS_ASSESSMENT_ATTENDANCE_H
#define STUDENTIS_ASSESSMENT_ATTENDANCE_H

#include <string>

namespace studentis {

class Lesson;
class Student;
class Teacher;

/**
 * @brief Отметка посещаемости: статус, опоздание, причина и отметивший преподаватель.
 *
 * Статусы: present, absent, excused, late.
 */
class Attendance {
public:
    Attendance() = default;
    Attendance(std::string id, Lesson* lesson, Student* student, std::string status,
               int minutesLate = 0, std::string reason = "", Teacher* markedBy = nullptr);

    /// Студент был на занятии (present или late).
    bool isPresent() const;
    /// Студент отсутствовал без уважительной причины.
    bool isAbsent() const;
    /// Отсутствие по уважительной причине.
    bool isExcused() const;
    /// Отметить уважительную причину; пустая причина — AttendanceException.
    void markExcused(const std::string& reason);
    /// Штраф за посещение: 0 для present/excused, минуты опоздания для late, 100 для absent.
    int latePenalty() const;
    /// Краткое текстовое описание отметки.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Lesson* getLesson() const { return lesson_; }
    Student* getStudent() const { return student_; }
    const std::string& getStatus() const { return status_; }
    int getMinutesLate() const { return minutesLate_; }
    const std::string& getReason() const { return reason_; }
    Teacher* getMarkedBy() const { return markedBy_; }

private:
    std::string id_;
    Lesson* lesson_ = nullptr;
    Student* student_ = nullptr;
    std::string status_ = "present";
    int minutesLate_ = 0;
    std::string reason_;
    Teacher* markedBy_ = nullptr;
};

} // namespace studentis

#endif
