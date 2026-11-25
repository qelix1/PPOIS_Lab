#ifndef STUDENTIS_CORE_ACADEMICYEAR_H
#define STUDENTIS_CORE_ACADEMICYEAR_H

#include <string>
#include <vector>

namespace studentis {

class Semester;

/**
 * @brief Учебный год: даты, семестры и признак текущего года.
 */
class AcademicYear {
public:
    AcademicYear() = default;
    AcademicYear(std::string name, std::string startDate, std::string endDate, bool current);

    /// Добавить семестр и связать его с годом; дубль — no-op.
    void addSemester(Semester& semester);
    /// Попадает ли дата в интервал учебного года.
    bool containsDate(const std::string& date) const;
    /// Найти семестр по номеру; nullptr, если нет.
    Semester* findSemester(int number) const;
    /// Признак текущего учебного года.
    bool isCurrent() const;
    /// Длительность учебного года в днях включительно.
    long durationDays() const;
    /// Завершить год: снять признак текущего и закрыть семестры.
    void close();

    const std::string& getName() const { return name_; }
    const std::string& getStartDate() const { return startDate_; }
    const std::string& getEndDate() const { return endDate_; }
    const std::vector<Semester*>& getSemesters() const { return semesters_; }

    void setCurrent(bool current) { current_ = current; }
    void setName(const std::string& name) { name_ = name; }
    void setStartDate(const std::string& startDate) { startDate_ = startDate; }
    void setEndDate(const std::string& endDate) { endDate_ = endDate; }

private:
    std::string name_;
    std::string startDate_;
    std::string endDate_;
    std::vector<Semester*> semesters_;
    bool current_ = false;
};

} // namespace studentis

#endif
