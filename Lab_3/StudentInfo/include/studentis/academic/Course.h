#ifndef STUDENTIS_ACADEMIC_COURSE_H
#define STUDENTIS_ACADEMIC_COURSE_H

#include <string>
#include <vector>

namespace studentis {

class Classroom;
class Discipline;
class Lesson;
class Semester;
class StudentGroup;
class Teacher;

/**
 * @brief Курс: дисциплина, преподаватель, группа, семестр и аудитория.
 */
class Course {
public:
    Course() = default;
    Course(std::string id, Discipline* discipline, Teacher* teacher, Semester* semester,
           StudentGroup* group, int plannedHours, Classroom* classroom, std::string status);

    /// Назначить преподавателя и закрепить дисциплину за ним.
    void assignTeacher(Teacher& teacher);
    /// Назначить аудиторию; группа не помещается — ClassroomCapacityException.
    void assignClassroom(Classroom& classroom);
    /// Привязать занятие к курсу с проверкой аудитории.
    void addLesson(Lesson& lesson);
    /// Процент выполненных часов (проведённые занятия) от плана.
    double completionPercent() const;
    /// Курс завершён: статус finished или план выполнен.
    bool isFinished() const;
    /// Число активных студентов группы.
    int studentsCount() const;
    /// Сменить статус: planned, active, finished, cancelled.
    void changeStatus(const std::string& status);
    /// Курс ведётся для указанной группы.
    bool matchesGroup(const StudentGroup& group) const;

    const std::string& getId() const { return id_; }
    Discipline* getDiscipline() const { return discipline_; }
    Teacher* getTeacher() const { return teacher_; }
    Semester* getSemester() const { return semester_; }
    StudentGroup* getGroup() const { return group_; }
    int getPlannedHours() const { return plannedHours_; }
    Classroom* getClassroom() const { return classroom_; }
    const std::string& getStatus() const { return status_; }
    const std::vector<Lesson*>& getLessons() const { return lessons_; }

    void setDiscipline(Discipline* discipline) { discipline_ = discipline; }
    void setSemester(Semester* semester) { semester_ = semester; }
    void setGroup(StudentGroup* group) { group_ = group; }
    void setPlannedHours(int plannedHours) { plannedHours_ = plannedHours; }

private:
    std::string id_;
    Discipline* discipline_ = nullptr;
    Teacher* teacher_ = nullptr;
    Semester* semester_ = nullptr;
    StudentGroup* group_ = nullptr;
    int plannedHours_ = 0;
    Classroom* classroom_ = nullptr;
    std::string status_ = "planned";
    // Приватный помощник: занятия курса нужны для расчёта выполнения плана.
    std::vector<Lesson*> lessons_;
};

} // namespace studentis

#endif
