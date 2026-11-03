#include "autoservice/core/CarModel.h"

#include "autoservice/core/CarBrand.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/inventory/PartCompatibility.h"

namespace autoservice {

CarModel::CarModel() = default;

CarModel::CarModel(std::string name, CarBrand* brand, std::string bodyType, int yearFrom, int yearTo)
    : name_(std::move(name)),
      brand_(brand),
      bodyType_(std::move(bodyType)),
      yearFrom_(yearFrom),
      yearTo_(yearTo) {}

bool CarModel::matchesYear(int year) const {
    if (yearFrom_ != 0 && year < yearFrom_) return false;
    if (yearTo_ != 0 && year > yearTo_) return false;
    return true;
}

void CarModel::addCompatibility(PartCompatibility& compatibility) {
    for (PartCompatibility* c : compatibilities_) {
        if (c == &compatibility) return;
    }
    compatibilities_.push_back(&compatibility);
}

bool CarModel::isCompatibleWith(const Part& part) const {
    for (const PartCompatibility* c : compatibilities_) {
        if (c && c->getPart() == &part) return true;
    }
    return false;
}

std::string CarModel::fullName() const {
    if (brand_) return brand_->getName() + " " + name_;
    return name_;
}

} // namespace autoservice
