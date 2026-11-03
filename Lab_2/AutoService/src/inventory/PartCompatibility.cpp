#include "autoservice/inventory/PartCompatibility.h"

#include "autoservice/core/CarModel.h"
#include "autoservice/inventory/Part.h"

namespace autoservice {

PartCompatibility::PartCompatibility() = default;

PartCompatibility::PartCompatibility(Part* part, CarModel* model, bool isUniversal)
    : part_(part), model_(model), isUniversal_(isUniversal) {}

bool PartCompatibility::fits(const CarModel& model) const {
    return isUniversal_ || model_ == &model;
}

void PartCompatibility::addNote(const std::string& note) {
    if (notes_.empty()) {
        notes_ = note;
    } else {
        notes_ += "; " + note;
    }
}

int PartCompatibility::compare(const PartCompatibility& other) const {
    std::string a = part_ ? part_->getSku() : "";
    std::string b = other.part_ ? other.part_->getSku() : "";
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

} // namespace autoservice
