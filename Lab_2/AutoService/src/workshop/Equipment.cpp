#include "autoservice/workshop/Equipment.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/workshop/ServiceBay.h"

namespace autoservice {

Equipment::Equipment() = default;

Equipment::Equipment(std::string id, std::string name, std::string purchasedAt, std::string calibrationDue,
                     int costRub)
    : id_(std::move(id)),
      name_(std::move(name)),
      purchasedAt_(std::move(purchasedAt)),
      calibrationDue_(std::move(calibrationDue)),
      costRub_(costRub) {}

void Equipment::startJob() {
    if (status_ == "working") throw WorkshopException("оборудование " + id_ + " уже в работе");
    if (status_ == "decommissioned")
        throw WorkshopException("списанное оборудование нельзя запустить: " + id_);
    status_ = "working";
}

void Equipment::stop() {
    if (status_ != "working") throw WorkshopException("оборудование " + id_ + " не в работе");
    status_ = "idle";
}

bool Equipment::isCalibrationDue(const std::string& today) const {
    return !calibrationDue_.empty() && today >= calibrationDue_;
}

void Equipment::bookToBay(ServiceBay& bay) {
    bay.addEquipment(*this);
    bay_ = &bay;
}

void Equipment::decommission(const std::string& reason) {
    if (reason.empty()) throw WorkshopException("нужна причина списания оборудования");
    decommissionReason_ = reason;
    status_ = "decommissioned";
}

} // namespace autoservice
