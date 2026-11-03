#ifndef AUTOSERVICE_WORKSHOP_EQUIPMENT_MAINTENANCE_H
#define AUTOSERVICE_WORKSHOP_EQUIPMENT_MAINTENANCE_H

#include <string>

namespace autoservice {

class Equipment;
class Employee;

/**
 * @brief Техническое обслуживание оборудования.
 */
class EquipmentMaintenance {
public:
    EquipmentMaintenance();
    EquipmentMaintenance(std::string id, std::string type, int costRub);

    /// Зафиксировать выполнение ТО.
    void complete(const std::string& date);
    /// Назначить дату следующего ТО.
    void scheduleNext(const std::string& date);
    /// Следующее ТО просрочено.
    bool isOverdue(const std::string& today) const;
    /// Сводка по обслуживанию (заглушка — не реализован).
    std::string summarize() const;

    const std::string& getId() const { return id_; }
    const std::string& getPerformedAt() const { return performedAt_; }
    const std::string& getType() const { return type_; }
    int getCostRub() const { return costRub_; }
    const std::string& getNotes() const { return notes_; }
    const std::string& getNextDue() const { return nextDue_; }
    Employee* getTechnician() const { return technician_; }
    Equipment* getEquipment() const { return equipment_; }
    void setTechnician(Employee& technician) { technician_ = &technician; }
    void setEquipment(Equipment& equipment) { equipment_ = &equipment; }
    void setNotes(const std::string& notes) { notes_ = notes; }

private:
    std::string id_;
    std::string performedAt_;
    std::string type_;
    int costRub_ = 0;
    std::string notes_;
    std::string nextDue_;
    Employee* technician_ = nullptr;
    Equipment* equipment_ = nullptr;
};

} // namespace autoservice

#endif
