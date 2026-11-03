#include "autoservice/staff/Director.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/workshop/ShopReport.h"
#include "autoservice/workshop/Workshop.h"

namespace autoservice {

Director::Director() = default;

Director::Director(std::string id, std::string fullName, std::string company)
    : Employee(std::move(id), std::move(fullName), "", date::today(), 0, "Директор", 0),
      company_(std::move(company)) {}

void Director::hireStaff(Employee& employee, Workshop& workshop) {
    employee.assignWorkshop(workshop);
    workshop.hire(employee);
}

void Director::dismissStaff(Employee& employee) {
    employee.dismiss();
}

void Director::approveBudget(int rub) {
    if (rub <= 0) throw InvalidAmountException("бюджет должен быть положительным");
    budgetRub_ = rub;
}

void Director::signReport(ShopReport& report) {
    report.markSigned(getFullName());
}

void Director::addWorkshop(Workshop& workshop) {
    for (Workshop* w : ownedWorkshops_) {
        if (w == &workshop) return;
    }
    ownedWorkshops_.push_back(&workshop);
}

} // namespace autoservice
