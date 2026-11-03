#ifndef AUTOSERVICE_WORKSHOP_EQUIPMENT_H
#define AUTOSERVICE_WORKSHOP_EQUIPMENT_H

#include <string>

namespace autoservice {

class ServiceBay;
class EquipmentMaintenance;

/**
 * @brief Оборудование мастерской: статус, поверка, стоимость.
 */
class Equipment {
public:
    Equipment();
    Equipment(std::string id, std::string name, std::string purchasedAt, std::string calibrationDue, int costRub);

    /// Запустить оборудование в работу.
    void startJob();
    /// Остановить оборудование.
    void stop();
    /// Подошла ли поверка/калибровка.
    bool isCalibrationDue(const std::string& today) const;
    /// Закрепить оборудование за постом.
    void bookToBay(ServiceBay& bay);
    /// Списать оборудование с эксплуатации.
    void decommission(const std::string& reason);

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getPurchasedAt() const { return purchasedAt_; }
    const std::string& getCalibrationDue() const { return calibrationDue_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getDecommissionReason() const { return decommissionReason_; }
    int getCostRub() const { return costRub_; }
    ServiceBay* getBay() const { return bay_; }
    EquipmentMaintenance* getMaintenance() const { return maintenance_; }
    void setMaintenance(EquipmentMaintenance& maintenance) { maintenance_ = &maintenance; }

private:
    std::string id_;
    std::string name_;
    std::string purchasedAt_;
    std::string calibrationDue_;
    std::string status_ = "idle";
    std::string decommissionReason_;
    int costRub_ = 0;
    ServiceBay* bay_ = nullptr;
    EquipmentMaintenance* maintenance_ = nullptr;
};

} // namespace autoservice

#endif
