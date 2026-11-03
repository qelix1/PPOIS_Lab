#include "autoservice/staff/Salary.h"

#include "autoservice/exceptions/InvalidAmountException.h"

namespace autoservice {

Salary::Salary() = default;

Salary::Salary(int baseRub, std::string periodStart, std::string periodEnd, Employee* employee)
    : baseRub_(baseRub),
      periodStart_(std::move(periodStart)),
      periodEnd_(std::move(periodEnd)),
      employee_(employee) {}

void Salary::accrueBonus(int rub) {
    if (rub <= 0) throw InvalidAmountException("премия должна быть положительной");
    bonusRub_ += rub;
}

void Salary::applyPenalty(int rub) {
    if (rub <= 0) throw InvalidAmountException("штраф должен быть положительным");
    if (penaltyRub_ + rub > baseRub_ + bonusRub_)
        throw InvalidAmountException("штраф превышает начисления за период");
    penaltyRub_ += rub;
}

int Salary::netPay() const {
    int pay = baseRub_ + bonusRub_ - penaltyRub_;
    return pay > 0 ? pay : 0;
}

bool Salary::isPayDay(const std::string& today) const {
    return today == periodEnd_;
}

} // namespace autoservice
