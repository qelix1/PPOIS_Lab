#ifndef AUTOSERVICE_CORE_CAR_MODEL_H
#define AUTOSERVICE_CORE_CAR_MODEL_H

#include <string>
#include <vector>

namespace autoservice {

class CarBrand;
class Part;
class PartCompatibility;

/**
 * @brief Модель автомобиля: кузов, годы выпуска, совместимые запчасти.
 */
class CarModel {
public:
    CarModel();
    CarModel(std::string name, CarBrand* brand, std::string bodyType, int yearFrom, int yearTo);

    /// Модель выпускалась в указанный год.
    bool matchesYear(int year) const;
    /// Добавить запись о совместимой запчасти.
    void addCompatibility(PartCompatibility& compatibility);
    /// Совместима ли запчасть с этой моделью.
    bool isCompatibleWith(const Part& part) const;
    /// Полное название: «марка модель».
    std::string fullName() const;

    const std::string& getName() const { return name_; }
    CarBrand* getBrand() const { return brand_; }
    const std::string& getBodyType() const { return bodyType_; }
    int getYearFrom() const { return yearFrom_; }
    int getYearTo() const { return yearTo_; }

private:
    std::string name_;
    CarBrand* brand_ = nullptr;
    std::string bodyType_;
    int yearFrom_ = 0;
    int yearTo_ = 0;
    std::vector<PartCompatibility*> compatibilities_;
};

} // namespace autoservice

#endif
