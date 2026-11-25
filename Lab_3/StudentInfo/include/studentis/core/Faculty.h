#ifndef STUDENTIS_CORE_FACULTY_H
#define STUDENTIS_CORE_FACULTY_H

#include <string>
#include <vector>

namespace studentis {

class Department;
class Specialty;
class Student;
class StudentGroup;
class Teacher;

/**
 * @brief Факультет: кафедры, декан и агрегированные показатели.
 */
class Faculty {
public:
    Faculty() = default;
    Faculty(std::string id, std::string name, std::string shortName, Teacher* dean,
            std::string phone, std::string building);

    /// Добавить кафедру и связать её с факультетом; дубль — no-op.
    void addDepartment(Department& department);
    /// Найти кафедру по идентификатору или названию; nullptr, если нет.
    Department* findDepartment(const std::string& idOrName) const;
    /// Специальности факультета из переданного списка.
    std::vector<Specialty*> allSpecialties(const std::vector<Specialty*>& specialties) const;
    /// Назначить декана.
    void appointDean(Teacher& dean);
    /// Число активных студентов факультета по переданному списку.
    int studentCount(const std::vector<Student*>& students) const;
    /// Число преподавателей на всех кафедрах факультета.
    int teacherCount() const;
    /// Переименовать факультет; оба пустых поля — ошибка данных.
    void rename(const std::string& name, const std::string& shortName);

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getShortName() const { return shortName_; }
    Teacher* getDean() const { return dean_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getBuilding() const { return building_; }
    const std::vector<Department*>& getDepartments() const { return departments_; }

    void setPhone(const std::string& phone) { phone_ = phone; }
    void setBuilding(const std::string& building) { building_ = building; }

private:
    std::string id_;
    std::string name_;
    std::string shortName_;
    Teacher* dean_ = nullptr;
    std::string phone_;
    std::string building_;
    std::vector<Department*> departments_;
};

} // namespace studentis

#endif
