#include "autoservice/workshop/ShopReport.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/workshop/ClientFeedback.h"

namespace autoservice {

ShopReport::ShopReport() = default;

ShopReport::ShopReport(std::string id, std::string periodStart, std::string periodEnd)
    : id_(std::move(id)), periodStart_(std::move(periodStart)), periodEnd_(std::move(periodEnd)) {}

void ShopReport::addOrder(int revenueRub) {
    if (revenueRub < 0) throw InvalidAmountException("выручка заказа не может быть отрицательной");
    revenueRub_ += revenueRub;
    ++ordersCompleted_;
}

void ShopReport::addFeedback(ClientFeedback& feedback) {
    for (ClientFeedback* f : feedbacks_) {
        if (f == &feedback) return;
    }
    feedbacks_.push_back(&feedback);
    recalcRating();
}

void ShopReport::recalcRating() {
    if (feedbacks_.empty()) {
        ratingAvg_ = 0.0;
        return;
    }
    double sum = 0.0;
    int count = 0;
    for (const ClientFeedback* f : feedbacks_) {
        if (f) {
            sum += static_cast<double>(f->getRating());
            ++count;
        }
    }
    ratingAvg_ = count == 0 ? 0.0 : sum / static_cast<double>(count);
}

double ShopReport::profitMargin(int expensesRub) const {
    if (revenueRub_ <= 0) return 0.0;
    return static_cast<double>(revenueRub_ - expensesRub) * 100.0 / static_cast<double>(revenueRub_);
}

void ShopReport::markSigned(const std::string& by) {
    if (by.empty()) throw WorkshopException("подпись отчёта не может быть пустой");
    signedBy_ = by;
}

std::string ShopReport::summary() const {
    throw WorkshopException("не реализован: текст сводки отчёта " + id_);
}

} // namespace autoservice
