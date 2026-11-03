#include "autoservice/staff/Employee.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

Employee::Employee() = default;

Employee::Employee(std::string id, std::string fullName, std::string phone, std::string hiredAt,
                   int baseSalaryRub, std::string role, int experienceYears)
    : id_(std::move(id)),
      fullName_(std::move(fullName)),
      phone_(std::move(phone)),
      hiredAt_(std::move(hiredAt)),
      baseSalaryRub_(baseSalaryRub),
      role_(std::move(role)),
      experienceYears_(experienceYears) {}

void Employee::startVacation(const std::string& from) {
    if (status_ == "dismissed") throw WorkshopException("уволенный сотрудник не может уйти в отпуск");
    if (status_ == "on_vacation") throw WorkshopException("сотрудник уже в отпуске до " + vacationUntil_);
    vacationUntil_ = from;
    status_ = "on_vacation";
}

void Employee::endVacation(const std::string& to) {
    if (status_ != "on_vacation") throw WorkshopException("сотрудник не в отпуске");
    (void)to;
    vacationUntil_.clear();
    status_ = "active";
}

int Employee::yearsWorked(int currentYear) const {
    if (hiredAt_.size() < 4) return 0;
    int hiredYear = 0;
    try {
        hiredYear = std::stoi(hiredAt_.substr(0, 4));
    } catch (const std::exception&) {
        return 0;
    }
    int years = currentYear - hiredYear;
    return years > 0 ? years : 0;
}

void Employee::raiseSalary(int percent) {
    if (percent <= 0 || percent > 100) throw InvalidAmountException("повышение оклада должно быть от 1 до 100 процентов");
    baseSalaryRub_ = baseSalaryRub_ * (100 + percent) / 100;
}

void Employee::assignWorkshop(Workshop& workshop) {
    workshop_ = &workshop;
}

void Employee::changeRole(const std::string& newRole) {
    if (newRole.empty()) throw WorkshopException("новая роль не может быть пустой");
    role_ = newRole;
}

void Employee::dismiss() {
    status_ = "dismissed";
    workshop_ = nullptr;
}

} // namespace autoservice
