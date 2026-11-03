#ifndef AUTOSERVICE_CORE_TRANSMISSION_H
#define AUTOSERVICE_CORE_TRANSMISSION_H

#include <string>

namespace autoservice {

class Vehicle;

/**
 * @brief Коробка передач автомобиля.
 */
class Transmission {
public:
    Transmission();
    Transmission(std::string type, int gearCount, std::string driveType, std::string manufacturer,
                 int serviceIntervalKm);

    /// Автоматическая коробка (АТ/РКП/вариатор).
    bool isAutomatic() const;
    /// Наступило ли обслуживание по пробегу.
    bool maintenanceDue(int mileageKm) const;
    /// Оценка ресурса по типу коробки, км.
    int expectedLifespanKm() const;
    /// Описание для заказа-наряда.
    std::string describe() const;
    /// Привязать коробку к автомобилю.
    void linkVehicle(Vehicle& vehicle);

    const std::string& getType() const { return type_; }
    int getGearCount() const { return gearCount_; }
    const std::string& getDriveType() const { return driveType_; }
    const std::string& getManufacturer() const { return manufacturer_; }
    int getServiceIntervalKm() const { return serviceIntervalKm_; }
    Vehicle* getVehicle() const { return vehicle_; }

private:
    std::string type_;
    int gearCount_ = 0;
    std::string driveType_;
    std::string manufacturer_;
    int serviceIntervalKm_ = 0;
    Vehicle* vehicle_ = nullptr;
};

} // namespace autoservice

#endif
