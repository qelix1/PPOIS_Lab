#ifndef AUTOSERVICE_CORE_CAR_BRAND_H
#define AUTOSERVICE_CORE_CAR_BRAND_H

#include <string>
#include <vector>

namespace autoservice {

class CarModel;

/**
 * @brief Автомобильная марка и её модельный ряд.
 */
class CarBrand {
public:
    CarBrand();
    CarBrand(std::string name, std::string country, int foundedYear);

    /// Добавить модель в модельный ряд марки.
    void addModel(CarModel& model);
    /// Найти модель по названию (nullptr — не найдена).
    CarModel* findModel(const std::string& name) const;
    /// Модели определённого типа кузова.
    std::vector<CarModel*> filterByBodyType(const std::string& bodyType) const;
    /// Модели, выпускавшиеся в заданный год.
    std::vector<CarModel*> modelsReleasedIn(int year) const;

    const std::string& getName() const { return name_; }
    const std::string& getCountry() const { return country_; }
    int getFoundedYear() const { return foundedYear_; }
    std::size_t getModelCount() const { return models_.size(); }

private:
    std::string name_;
    std::string country_;
    int foundedYear_ = 0;
    std::vector<CarModel*> models_;
};

} // namespace autoservice

#endif
