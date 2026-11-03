#include "autoservice/core/CarBrand.h"

#include "autoservice/core/CarModel.h"

namespace autoservice {

CarBrand::CarBrand() = default;

CarBrand::CarBrand(std::string name, std::string country, int foundedYear)
    : name_(std::move(name)), country_(std::move(country)), foundedYear_(foundedYear) {}

void CarBrand::addModel(CarModel& model) {
    for (CarModel* m : models_) {
        if (m == &model) return;
    }
    models_.push_back(&model);
}

CarModel* CarBrand::findModel(const std::string& name) const {
    for (CarModel* m : models_) {
        if (m && m->getName() == name) return m;
    }
    return nullptr;
}

std::vector<CarModel*> CarBrand::filterByBodyType(const std::string& bodyType) const {
    std::vector<CarModel*> result;
    for (CarModel* m : models_) {
        if (m && m->getBodyType() == bodyType) result.push_back(m);
    }
    return result;
}

std::vector<CarModel*> CarBrand::modelsReleasedIn(int year) const {
    std::vector<CarModel*> result;
    for (CarModel* m : models_) {
        if (m && m->matchesYear(year)) result.push_back(m);
    }
    return result;
}

} // namespace autoservice
