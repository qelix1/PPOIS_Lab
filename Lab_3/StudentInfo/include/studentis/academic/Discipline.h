#ifndef STUDENTIS_ACADEMIC_DISCIPLINE_H
#define STUDENTIS_ACADEMIC_DISCIPLINE_H

#include <string>
#include <vector>

namespace studentis {

class Department;
class Student;

/**
 * @brief Учебная дисциплина: код, трудоёмкость и пререквизиты.
 */
class Discipline {
public:
    Discipline() = default;
    Discipline(std::string id, std::string name, std::string code, Department* department,
               int credits, std::string description);

    /// Добавить пререквизит; дубль — no-op, сама дисциплина — ошибка данных.
    void addPrerequisite(Discipline& prerequisite);
    /// Является ли дисциплина пререквизитом текущей.
    bool hasPrerequisite(const Discipline& prerequisite) const;
    /// Можно ли изучать дисциплину: все пререквизиты сданы студентом.
    bool canStudy(const Student& student) const;
    /// Совпадение запроса с названием или кодом без учёта регистра.
    bool matchesName(const std::string& query) const;
    /// Закрепить дисциплину за кафедрой.
    void assignDepartment(Department& department);
    /// Число пререквизитов.
    int totalPrerequisites() const;
    /// Краткое текстовое описание дисциплины.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getCode() const { return code_; }
    Department* getDepartment() const { return department_; }
    int getCredits() const { return credits_; }
    const std::string& getDescription() const { return description_; }
    const std::vector<Discipline*>& getPrerequisites() const { return prerequisites_; }

    void setName(const std::string& name) { name_ = name; }
    void setCredits(int credits) { credits_ = credits; }
    void setDescription(const std::string& description) { description_ = description; }

private:
    std::string id_;
    std::string name_;
    std::string code_;
    Department* department_ = nullptr;
    int credits_ = 0;
    std::string description_;
    std::vector<Discipline*> prerequisites_;
};

} // namespace studentis

#endif
