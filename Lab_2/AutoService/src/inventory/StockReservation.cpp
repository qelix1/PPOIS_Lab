#include "autoservice/inventory/StockReservation.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/Part.h"

namespace autoservice {

StockReservation::StockReservation() = default;

StockReservation::StockReservation(std::string id, Part* part, int quantity, RepairOrder* order,
                                   std::string expiresAt)
    : id_(std::move(id)),
      part_(part),
      quantity_(quantity),
      order_(order),
      expiresAt_(std::move(expiresAt)) {}

void StockReservation::confirm() {
    if (status_ != "active")
        throw WorkshopException("подтвердить можно только активный резерв, статус: " + status_);
    if (!part_) throw WorkshopException("резерв без запчасти");
    part_->reserve(quantity_);
    status_ = "confirmed";
}

void StockReservation::cancel() {
    if (status_ == "cancelled") throw WorkshopException("резерв уже отменён");
    if (status_ == "confirmed" && part_) part_->release(quantity_);
    status_ = "cancelled";
}

bool StockReservation::isExpired(const std::string& today) const {
    return status_ == "active" && date::daysBetween(expiresAt_, today) > 0;
}

void StockReservation::extend(const std::string& newDate) {
    if (status_ != "active")
        throw WorkshopException("продлить можно только активный резерв, статус: " + status_);
    expiresAt_ = newDate;
}

} // namespace autoservice
