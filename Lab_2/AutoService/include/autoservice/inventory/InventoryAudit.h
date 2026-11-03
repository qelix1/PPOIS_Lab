#ifndef AUTOSERVICE_INVENTORY_INVENTORY_AUDIT_H
#define AUTOSERVICE_INVENTORY_INVENTORY_AUDIT_H

#include <string>
#include <vector>

namespace autoservice {

class InventoryLine;
class Employee;
class Warehouse;

/**
 * @brief Инвентаризация склада: сверка остатков по строкам.
 */
class InventoryAudit {
public:
    InventoryAudit();
    InventoryAudit(std::string id, std::string startedAt);

    /// Добавить строку сверки.
    void addLine(InventoryLine& line);
    /// Завершить инвентаризацию и подсчитать расхождения.
    void finish(const std::string& date);
    /// Сумма абсолютных расхождений по количеству.
    int totalDiscrepancy() const;
    /// Есть ли расхождения с учётом.
    bool hasIssues() const;
    /// Подписать акт инвентаризации.
    void sign(Employee& auditor);

    const std::string& getId() const { return id_; }
    const std::string& getStartedAt() const { return startedAt_; }
    const std::string& getFinishedAt() const { return finishedAt_; }
    const std::vector<InventoryLine*>& getLines() const { return lines_; }
    Employee* getAuditor() const { return auditor_; }
    int getDiscrepancies() const { return discrepancies_; }
    Warehouse* getWarehouse() const { return warehouse_; }
    void setWarehouse(Warehouse& warehouse) { warehouse_ = &warehouse; }
    void setAuditor(Employee& auditor) { auditor_ = &auditor; }

private:
    std::string id_;
    std::string startedAt_;
    std::string finishedAt_;
    std::vector<InventoryLine*> lines_;
    Employee* auditor_ = nullptr;
    int discrepancies_ = 0;
    Warehouse* warehouse_ = nullptr;
};

} // namespace autoservice

#endif
