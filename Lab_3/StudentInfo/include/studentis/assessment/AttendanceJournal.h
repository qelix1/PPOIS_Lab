#ifndef STUDENTIS_ASSESSMENT_ATTENDANCEJOURNAL_H
#define STUDENTIS_ASSESSMENT_ATTENDANCEJOURNAL_H

#include <string>
#include <vector>

namespace studentis {

class Attendance;
class Course;
class Lesson;
class Student;
class StudentGroup;

/**
 * @brief Журнал посещаемости группы по курсу.
 *
 * Процент посещаемости считается по статусам present, late и excused.
 */
class AttendanceJournal {
public:
    AttendanceJournal() = default;
    AttendanceJournal(StudentGroup* group, Course* course);

    /// Отметить посещаемость; повторная отметка заменяет прежнюю, некорректная — AttendanceException.
    void mark(Attendance& entry);
    /// Процент посещаемости по всем отметкам (0..100).
    double attendanceRate() const;
    /// Число пропусков без уважительной причины.
    int absentCount() const;
    /// Число пропусков без уважительной причины у студента.
    int unexcusedAbsences(const Student& student) const;
    /// Студенты с числом неуважительных пропусков не меньше порога.
    std::vector<Student*> chronicAbsentees(int minAbsences) const;
    /// Процент посещаемости по конкретному занятию (0..100).
    double lessonRate(const Lesson& lesson) const;

    StudentGroup* getGroup() const { return group_; }
    Course* getCourse() const { return course_; }
    const std::vector<Attendance*>& getEntries() const { return entries_; }

private:
    StudentGroup* group_ = nullptr;
    Course* course_ = nullptr;
    std::vector<Attendance*> entries_;
};

} // namespace studentis

#endif
