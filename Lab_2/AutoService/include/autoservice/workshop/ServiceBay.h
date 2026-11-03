#ifndef AUTOSERVICE_WORKSHOP_SERVICE_BAY_H
#define AUTOSERVICE_WORKSHOP_SERVICE_BAY_H

#include <string>
#include <vector>

namespace autoservice {

class RepairOrder;
class Equipment;
class Workshop;
class Appointment;

/**
 * @brief Пост обслуживания (бокс): занятость, оборудование, грузоподъёмность.
 */
class ServiceBay {
public:
    ServiceBay();
    ServiceBay(std::string number, std::string type, int maxWeightKg);

    /// Занять пост заказом; занятый пост — ServiceBayBusyException.
    void assign(RepairOrder& order);
    /// Освободить пост.
    void free();
    /// Пост сейчас занят.
    bool isBusy() const;
    /// Пост может принять заказ (свободен).
    bool canTake(const RepairOrder& order) const;
    /// Отчёт по простою поста (заглушка — не реализован).
    std::string downtimeReport() const;
    /// Закрепить оборудование за постом.
    void addEquipment(Equipment& equipment);

    const std::string& getNumber() const { return number_; }
    const std::string& getType() const { return type_; }
    const std::string& getStatus() const { return status_; }
    RepairOrder* getCurrentOrder() const { return currentOrder_; }
    const std::vector<Equipment*>& getEquipment() const { return equipment_; }
    int getMaxWeightKg() const { return maxWeightKg_; }
    Workshop* getWorkshop() const { return workshop_; }
    void setWorkshop(Workshop& workshop) { workshop_ = &workshop; }

private:
    std::string number_;
    std::string type_;
    std::string status_ = "free";
    RepairOrder* currentOrder_ = nullptr;
    std::vector<Equipment*> equipment_;
    int maxWeightKg_ = 0;
    Workshop* workshop_ = nullptr;
};

} // namespace autoservice

#endif
