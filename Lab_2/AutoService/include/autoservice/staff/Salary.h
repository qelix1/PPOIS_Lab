#ifndef AUTOSERVICE_STAFF_SALARY_H
#define AUTOSERVICE_STAFF_SALARY_H

#include <string>

namespace autoservice {

class Employee;

/**
 * @brief Расчётный лист сотрудника за период.
 */
class Salary {
public:
    Salary();
    Salary(int baseRub, std::string periodStart, std::string periodEnd, Employee* employee);

    /// Начислить премию.
    void accrueBonus(int rub);
    /// Удержать штраф (не больше начислений).
    void applyPenalty(int rub);
    /// К выплате: оклад + премия − штраф (не меньше нуля).
    int netPay() const;
    /// Сегодняшняя дата совпадает с окончанием расчётного периода.
    bool isPayDay(const std::string& today) const;

    int getBaseRub() const { return baseRub_; }
    int getBonusRub() const { return bonusRub_; }
    int getPenaltyRub() const { return penaltyRub_; }
    const std::string& getPeriodStart() const { return periodStart_; }
    const std::string& getPeriodEnd() const { return periodEnd_; }
    Employee* getEmployee() const { return employee_; }

private:
    int baseRub_ = 0;
    int bonusRub_ = 0;
    int penaltyRub_ = 0;
    std::string periodStart_;
    std::string periodEnd_;
    Employee* employee_ = nullptr;
};

} // namespace autoservice

#endif
