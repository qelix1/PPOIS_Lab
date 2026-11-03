#ifndef AUTOSERVICE_STAFF_DIRECTOR_H
#define AUTOSERVICE_STAFF_DIRECTOR_H

#include <string>
#include <vector>

#include "autoservice/staff/Employee.h"

namespace autoservice {

class Workshop;
class ShopReport;

/**
 * @brief Директор: кадры, бюджет, отчётность по площадкам.
 */
class Director : public Employee {
public:
    Director();
    Director(std::string id, std::string fullName, std::string company);

    /// Принять сотрудника в штат мастерской.
    void hireStaff(Employee& employee, Workshop& workshop);
    /// Уволить сотрудника.
    void dismissStaff(Employee& employee);
    /// Утвердить бюджет ( положительная сумма ).
    void approveBudget(int rub);
    /// Подписать отчёт по мастерской.
    void signReport(ShopReport& report);
    /// Взять площадку под управление.
    void addWorkshop(Workshop& workshop);

    const std::string& getCompany() const { return company_; }
    const std::string& getStrategyNote() const { return strategyNote_; }
    int getBudgetRub() const { return budgetRub_; }
    std::size_t getWorkshopCount() const { return ownedWorkshops_.size(); }
    void setStrategyNote(const std::string& note) { strategyNote_ = note; }

private:
    std::string company_;
    std::string strategyNote_;
    int budgetRub_ = 0;
    std::vector<Workshop*> ownedWorkshops_;
};

} // namespace autoservice

#endif
