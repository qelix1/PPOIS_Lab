#include "autoservice/orders/WorkItem.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

WorkItem::WorkItem() = default;

WorkItem::WorkItem(std::string id, int plannedHours, int priceRub)
    : id_(std::move(id)), plannedHours_(plannedHours), priceRub_(priceRub) {}

void WorkItem::start() {
    if (status_ == "in_progress") throw InvalidOrderStatusException("работа уже выполняется");
    if (status_ == "done") throw InvalidOrderStatusException("завершённую работу нельзя начать заново");
    status_ = "in_progress";
}

void WorkItem::complete(int hoursWorked) {
    if (hoursWorked <= 0) throw InvalidAmountException("фактические часы должны быть положительными");
    if (status_ == "done") throw InvalidOrderStatusException("работа уже завершена");
    actualHours_ = hoursWorked;
    status_ = "done";
}

void WorkItem::pause(const std::string& reason) {
    if (status_ != "in_progress") throw InvalidOrderStatusException("приостановить можно только выполняющуюся работу");
    if (reason.empty()) throw WorkshopException("нужна причина приостановки работы");
    status_ = "paused";
}

void WorkItem::recalcPrice(int laborRateRubPerHour) {
    if (laborRateRubPerHour <= 0) throw InvalidAmountException("ставка руб/час должна быть положительной");
    priceRub_ = laborRateRubPerHour * plannedHours_;
}

int WorkItem::estimatedCost(int laborRateRubPerHour) const {
    if (laborRateRubPerHour < 0) return 0;
    return laborRateRubPerHour * plannedHours_;
}

std::string WorkItem::progressReport() const {
    throw WorkshopException("не реализовано: отчёт о ходе работ");
}

} // namespace autoservice
