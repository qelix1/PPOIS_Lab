#ifndef AUTOSERVICE_STAFF_WORK_SCHEDULE_H
#define AUTOSERVICE_STAFF_WORK_SCHEDULE_H

#include <string>
#include <vector>

namespace autoservice {

class Employee;
class Shift;
class Manager;

/**
 * @brief График смен на неделю.
 */
class WorkSchedule {
public:
    WorkSchedule();
    WorkSchedule(int weekNumber, std::string publishedAt, int version);

    /// Добавить смену в график.
    void addShift(Shift& shift);
    /// Убрать смену из графика.
    void removeShift(Shift& shift);
    /// Свободен ли сотрудник в указанный день (нет смен).
    bool isEmployeeFree(Employee& employee, const std::string& day) const;
    /// Суммарные часы сотрудника за неделю.
    int hoursPerWeek(Employee& employee) const;

    int getWeekNumber() const { return weekNumber_; }
    int getVersion() const { return version_; }
    const std::string& getPublishedAt() const { return publishedAt_; }
    Employee* getApprovedBy() const { return approvedBy_; }
    void approve(Employee& approver) { approvedBy_ = &approver; }
    std::size_t getShiftCount() const { return shifts_.size(); }

private:
    int weekNumber_ = 0;
    std::vector<Shift*> shifts_;
    std::string publishedAt_;
    Employee* approvedBy_ = nullptr;
    int version_ = 1;
};

} // namespace autoservice

#endif
