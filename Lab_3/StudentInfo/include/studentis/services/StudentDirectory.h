#ifndef STUDENTIS_SERVICES_STUDENTDIRECTORY_H
#define STUDENTIS_SERVICES_STUDENTDIRECTORY_H

#include <string>
#include <vector>

namespace studentis {

class Student;
class StudentGroup;
class Teacher;

/**
 * @brief Справочник студентов, групп и преподавателей с поиском и выгрузкой.
 */
class StudentDirectory {
public:
    StudentDirectory() = default;

    /// Добавить студента; дубль — no-op.
    void addStudent(Student& student);
    /// Добавить группу; дубль — no-op.
    void addGroup(StudentGroup& group);
    /// Добавить преподавателя; дубль — no-op.
    void addTeacher(Teacher& teacher);
    /// Найти студента по номеру или nullptr.
    Student* findById(const std::string& studentId) const;
    /// Найти студента по ФИО без учёта регистра или nullptr.
    Student* findByName(const std::string& query) const;
    /// Студенты указанной группы.
    std::vector<Student*> filterByGroup(const std::string& groupCode) const;
    /// Студенты указанного курса.
    std::vector<Student*> filterByCourse(int courseNumber) const;
    /// Лучшие активные студенты по среднему баллу.
    std::vector<Student*> topStudents(int count) const;
    /// Активные студенты с академическими задолженностями.
    std::vector<Student*> studentsWithDebt() const;
    /// Выгрузка всех студентов в CSV.
    std::string exportCsv() const;

    const std::vector<Student*>& getStudents() const { return students_; }
    const std::vector<StudentGroup*>& getGroups() const { return groups_; }
    const std::vector<Teacher*>& getTeachers() const { return teachers_; }

private:
    std::vector<Student*> students_;
    std::vector<StudentGroup*> groups_;
    std::vector<Teacher*> teachers_;
};

} // namespace studentis

#endif
