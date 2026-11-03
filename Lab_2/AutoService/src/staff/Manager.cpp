#include "autoservice/staff/Manager.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/finance/Invoice.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/staff/Mechanic.h"
#include "autoservice/workshop/MarketingCampaign.h"

namespace autoservice {

Manager::Manager() = default;

Manager::Manager(std::string id, std::string fullName, std::string region, int discountLimitPercent)
    : Employee(std::move(id), std::move(fullName), "", date::today(), 120000, "Менеджер", 0),
      discountLimitPercent_(discountLimitPercent),
      region_(std::move(region)) {}

void Manager::approveDiscount(Discount& discount) {
    if (discount.getPercent() > discountLimitPercent_)
        throw InvalidAmountException("скидка превышает лимит полномочий менеджера");
}

void Manager::assignMechanic(RepairOrder& order, Mechanic& mechanic) {
    mechanic.acceptOrder(order);
}

void Manager::cancelOrder(RepairOrder& order) {
    order.changeStatus("cancelled");
}

void Manager::releaseInvoice(Invoice& invoice) {
    invoice.issue();
}

void Manager::startCampaign(MarketingCampaign& campaign) {
    if (campaign.getBudgetRub() <= 0)
        throw InvalidAmountException("бюджет акции должен быть положительным");
    if (campaign.status(date::today()) == "ended")
        throw WorkshopException("нельзя запустить уже завершённую акцию");
}

double Manager::teamProductivity(const std::vector<Mechanic*>& mechanics) const {
    if (mechanics.empty()) return 0.0;
    double sum = 0.0;
    int count = 0;
    for (const Mechanic* m : mechanics) {
        if (m) {
            sum += m->getProductivityScore();
            ++count;
        }
    }
    return count == 0 ? 0.0 : sum / count;
}

} // namespace autoservice
