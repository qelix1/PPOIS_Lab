#ifndef STUDENTIS_CORE_DEPARTMENT_H
#define STUDENTIS_CORE_DEPARTMENT_H

#include <string>
#include <vector>

namespace studentis {

class Discipline;
class Faculty;
class Teacher;

/**
 * @brief Кафедра: преподаватели, дисциплины и заведующий.
 */
class Department {
public:
    Department() = default;
    Department(std::string id, std::string name, Faculty* faculty, Teacher* head, std::string phone);

    /// Добавить преподавателя и связать его с кафедрой; дубль — no-op.
    void addTeacher(Teacher& teacher);
    /// Убрать преподавателя по номеру; true — если он был на кафедре.
    bool removeTeacher(const std::string& teacherId);
    /// Закрепить дисциплину за кафедрой; дубль — no-op.
    void assignDiscipline(Discipline& discipline);
    /// Преподаватели с учёной степенью.
    std::vector<Teacher*> teachersWithDegree() const;
    /// Назначить заведующего кафедрой.
    void appointHead(Teacher& head);

    /// Найти дисциплину кафедры по идентификатору; nullptr, если нет.
    Discipline* findDiscipline(const std::string& disciplineId) const;
    /// Суммарная плановая нагрузка по дисциплинам кафедры, часов.
    int totalPlannedWorkload() const;

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    Faculty* getFaculty() const { return faculty_; }
    Teacher* getHead() const { return head_; }
    const std::string& getPhone() const { return phone_; }
    const std::vector<Teacher*>& getTeachers() const { return teachers_; }
    const std::vector<Discipline*>& getDisciplines() const { return disciplines_; }

    void setFaculty(Faculty* faculty) { faculty_ = faculty; }
    void setPhone(const std::string& phone) { phone_ = phone; }
    void setName(const std::string& name) { name_ = name; }

private:
    std::string id_;
    std::string name_;
    Faculty* faculty_ = nullptr;
    Teacher* head_ = nullptr;
    std::string phone_;
    std::vector<Teacher*> teachers_;
    std::vector<Discipline*> disciplines_;
};

} // namespace studentis

#endif
