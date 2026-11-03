#ifndef AUTOSERVICE_CORE_ENGINE_H
#define AUTOSERVICE_CORE_ENGINE_H

#include <string>

namespace autoservice {

class Vehicle;

/**
 * @brief Двигатель автомобиля: тип, объём, мощность.
 */
class Engine {
public:
    Engine();
    Engine(std::string engineCode, std::string type, double displacementL, int powerHp, int torqueNm,
           std::string fuelType);

    /// Удельная мощность, л.с. на литр объёма.
    double powerPerLiter() const;
    /// Двигатель с турбонаддувом.
    bool isTurbo() const;
    /// Наступило ли замена масла по пробегу.
    bool needsOilChange(int mileageKm, int intervalKm) const;
    /// Сводная строка характеристик для отчёта.
    std::string specSummary() const;
    /// Привязать двигатель к автомобилю.
    void linkVehicle(Vehicle& vehicle);

    const std::string& getEngineCode() const { return engineCode_; }
    const std::string& getType() const { return type_; }
    double getDisplacementL() const { return displacementL_; }
    int getPowerHp() const { return powerHp_; }
    int getTorqueNm() const { return torqueNm_; }
    const std::string& getFuelType() const { return fuelType_; }
    Vehicle* getVehicle() const { return vehicle_; }

private:
    std::string engineCode_;
    std::string type_;
    double displacementL_ = 0.0;
    int powerHp_ = 0;
    int torqueNm_ = 0;
    std::string fuelType_;
    Vehicle* vehicle_ = nullptr;
};

} // namespace autoservice

#endif
