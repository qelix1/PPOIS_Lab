#ifndef AUTOSERVICE_CORE_VEHICLE_H
#define AUTOSERVICE_CORE_VEHICLE_H

#include <string>

namespace autoservice {

class Client;
class CarBrand;
class CarModel;
class Engine;
class Transmission;

/**
 * @brief Автомобиль, принятый в мастерскую: VIN, пробег, узлы, владелец.
 */
class Vehicle {
public:
    Vehicle();
    Vehicle(std::string vin, int year, std::string color);

    /// Проверить VIN по правилам ISO 3779; при ошибке — InvalidVinException.
    void validateVin() const;
    /// Увеличить пробег; некорректное значение — InvalidAmountException.
    void increaseMileage(int km);
    /// Наступило ли техобслуживание по пробегу относительно последнего ТО.
    bool needsServiceByMileage(int intervalKm) const;
    /// Автомобиль пригоден к эксплуатации по возрасту и пробегу.
    bool isRoadworthy(int currentYear) const;
    /// Среднемесячный пробег за период владения (мес > 0).
    double averageMonthlyMileage(int ownershipMonths) const;
    /// Назначить владельца и зарегистрировать автомобиль у клиента.
    void assignOwner(Client& owner);
    /// Установить двигатель.
    void attachEngine(Engine& engine);
    /// Установить коробку передач.
    void attachTransmission(Transmission& transmission);

    const std::string& getVin() const { return vin_; }
    int getYear() const { return year_; }
    int getMileageKm() const { return mileageKm_; }
    int getLastServiceKm() const { return lastServiceKm_; }
    Client* getOwner() const { return owner_; }
    CarBrand* getBrand() const { return brand_; }
    CarModel* getModel() const { return model_; }
    Engine* getEngine() const { return engine_; }
    Transmission* getTransmission() const { return transmission_; }
    void setBrand(CarBrand& brand) { brand_ = &brand; }
    void setModel(CarModel& model) { model_ = &model; }
    void setMileage(int km) { mileageKm_ = km; }
    void setLastServiceKm(int km) { lastServiceKm_ = km; }

private:
    std::string vin_;
    CarBrand* brand_ = nullptr;
    CarModel* model_ = nullptr;
    int year_ = 0;
    int mileageKm_ = 0;
    std::string color_;
    Client* owner_ = nullptr;
    Engine* engine_ = nullptr;
    Transmission* transmission_ = nullptr;
    int lastServiceKm_ = 0;
};

} // namespace autoservice

#endif
