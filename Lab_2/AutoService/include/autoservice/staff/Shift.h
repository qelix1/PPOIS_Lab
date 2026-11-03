#ifndef AUTOSERVICE_STAFF_SHIFT_H
#define AUTOSERVICE_STAFF_SHIFT_H

#include <string>

namespace autoservice {

class Employee;
class ServiceBay;

/**
 * @brief Смена сотрудника: интервал дат-времён, пост и исполнитель.
 * Формат времени: «YYYY-MM-DD HH:MM» (лексикографическое сравнение = хронологическое).
 */
class Shift {
public:
    Shift();
    Shift(std::string startsAt, std::string endsAt, Employee* employee, ServiceBay* bay);

    /// Пересечение интервалов с другой сменой.
    bool overlapsWith(const Shift& other) const;
    /// Продолжительность смены в минутах.
    int durationMinutes() const;
    /// Смена уже завершена на указанный момент.
    bool isFinished(const std::string& now) const;

    const std::string& getStartsAt() const { return startsAt_; }
    const std::string& getEndsAt() const { return endsAt_; }
    Employee* getEmployee() const { return employee_; }
    ServiceBay* getBay() const { return bay_; }
    void setEmployee(Employee& employee) { employee_ = &employee; }

private:
    std::string startsAt_;
    std::string endsAt_;
    Employee* employee_ = nullptr;
    ServiceBay* bay_ = nullptr;
};

} // namespace autoservice

#endif
