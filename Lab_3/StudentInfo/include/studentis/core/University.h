#ifndef STUDENTIS_CORE_UNIVERSITY_H
#define STUDENTIS_CORE_UNIVERSITY_H

#include <string>
#include <vector>

namespace studentis {

class AcademicYear;
class Faculty;
class Student;
class StudentGroup;
class Teacher;

/**
 * @brief Университет: факультеты, студенты, ректор и текущий учебный год.
 */
class University {
public:
    University() = default;
    University(std::string name, Teacher* rector, int foundedYear);

    /// Добавить факультет; дубль — no-op.
    void addFaculty(Faculty& faculty);
    /// Найти факультет по идентификатору или названию; nullptr, если нет.
    Faculty* findFaculty(const std::string& idOrName) const;
    /// Зачислить студента в университет; дубль — no-op.
    void enrollStudent(Student& student);
    /// Отчислить студента по номеру; неизвестный номер — StudentNotFoundException.
    void expelStudent(const std::string& studentId);
    /// Число активных студентов университета.
    int totalStudents() const;
    /// Найти группу по коду среди групп зачисленных студентов; nullptr, если нет.
    StudentGroup* findGroup(const std::string& code) const;
    /// Назначить текущий учебный год (предыдущий перестаёт быть текущим).
    void setCurrentYear(AcademicYear& year);
    /// Краткая сводка по университету.
    std::string summary() const;

    const std::string& getName() const { return name_; }
    Teacher* getRector() const { return rector_; }
    int getFoundedYear() const { return foundedYear_; }
    AcademicYear* getCurrentYear() const { return currentYear_; }
    const std::vector<Faculty*>& getFaculties() const { return faculties_; }
    const std::vector<Student*>& getStudents() const { return students_; }

    void setName(const std::string& name) { name_ = name; }
    void setRector(Teacher* rector) { rector_ = rector; }

private:
    std::string name_;
    Teacher* rector_ = nullptr;
    int foundedYear_ = 0;
    std::vector<Faculty*> faculties_;
    std::vector<Student*> students_;
    AcademicYear* currentYear_ = nullptr;
};

} // namespace studentis

#endif
