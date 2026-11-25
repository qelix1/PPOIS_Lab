#ifndef STUDENTIS_CORE_TEACHER_H
#define STUDENTIS_CORE_TEACHER_H

#include <string>
#include <vector>

#include "studentis/core/Person.h"

namespace studentis {

class Department;
class Discipline;
class Grade;
class Thesis;

/**
 * @brief Преподаватель: кафедра, должность, степень, нагрузка и дисциплины.
 */
class Teacher : public Person {
public:
    Teacher() = default;
    Teacher(std::string id, std::string lastName, std::string firstName, std::string middleName,
            std::string birthDate, std::string gender, std::string phone, std::string email,
            std::string address, std::string teacherId, std::string position,
            std::string academicDegree, std::string academicTitle, std::string hiredAt,
            int baseWorkloadHours);

    /// Закрепить дисциплину за преподавателем (повторный вызов — no-op).
    void assignDiscipline(Discipline& discipline);
    /// Снять дисциплину; отсутствующая дисциплина — no-op.
    void removeDiscipline(const Discipline& discipline);
    /// Ведёт ли преподаватель указанную дисциплину.
    bool canTeach(const Discipline& discipline) const;
    /// Профессор ли: должность или учёное звание professor.
    bool isProfessor() const;
    /// Полных лет стажа на указанную дату.
    int yearsOfService(const std::string& date) const;

    /// Плановая нагрузка с учётом закреплённых дисциплин, часов.
    int plannedWorkload() const;
    /// Научное руководство работой студента.
    void superviseThesis(Thesis& thesis);
    /// Выставить оценку студенту.
    void markGrade(Grade& grade);
    /// Загрузка в процентах от базовой нормы.
    int workloadPercent() const;

    const std::string& getTeacherId() const { return teacherId_; }
    Department* getDepartment() const { return department_; }
    const std::string& getPosition() const { return position_; }
    const std::string& getAcademicDegree() const { return academicDegree_; }
    const std::string& getAcademicTitle() const { return academicTitle_; }
    const std::string& getHiredAt() const { return hiredAt_; }
    int getBaseWorkloadHours() const { return baseWorkloadHours_; }
    const std::vector<Discipline*>& getDisciplines() const { return disciplines_; }

    void setDepartment(Department* department) { department_ = department; }
    void setPosition(const std::string& position) { position_ = position; }
    void setAcademicDegree(const std::string& academicDegree) { academicDegree_ = academicDegree; }
    void setAcademicTitle(const std::string& academicTitle) { academicTitle_ = academicTitle; }
    void setBaseWorkloadHours(int baseWorkloadHours) { baseWorkloadHours_ = baseWorkloadHours; }

private:
    std::string teacherId_;
    Department* department_ = nullptr;
    std::string position_;
    std::string academicDegree_;
    std::string academicTitle_;
    std::string hiredAt_;
    int baseWorkloadHours_ = 0;
    std::vector<Discipline*> disciplines_;
};

} // namespace studentis

#endif
