#include "autoservice/core/VehicleDocument.h"

#include "autoservice/core/DateUtils.h"

namespace autoservice {

VehicleDocument::VehicleDocument() = default;

VehicleDocument::VehicleDocument(std::string docType, std::string number, std::string issuedAt,
                                 std::string expiresAt)
    : docType_(std::move(docType)),
      number_(std::move(number)),
      issuedAt_(std::move(issuedAt)),
      expiresAt_(std::move(expiresAt)) {}

bool VehicleDocument::isExpired(const std::string& today) const {
    return date::daysBetween(expiresAt_, today) > 0;
}

long VehicleDocument::daysUntilExpiry(const std::string& today) const {
    return date::daysBetween(today, expiresAt_);
}

void VehicleDocument::transferTo(Client& newOwner) {
    owner_ = &newOwner;
}

bool VehicleDocument::isValid(const std::string& today) const {
    return vehicle_ != nullptr && owner_ != nullptr && !isExpired(today);
}

} // namespace autoservice
