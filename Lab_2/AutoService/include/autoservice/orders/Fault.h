#ifndef AUTOSERVICE_ORDERS_FAULT_H
#define AUTOSERVICE_ORDERS_FAULT_H

#include <string>

namespace autoservice {

class DiagnosticReport;
class RepairOrder;

/**
 * @brief Дефект/неисправность автомобиля (шаг 1..5 по тяжести).
 */
class Fault {
public:
    Fault();
    Fault(std::string code, std::string description, int severity, std::string unit, std::string foundAt);

    /// Повысить тяжесть дефекта (максимум — 5).
    void escalate();
    /// Отметить дефект устранённым.
    void markFixed(const std::string& date);
    /// Критический дефект: тяжесть ≥ 4.
    bool isCritical() const;
    /// Сколько дней дефект открыт (до фиксации или до сегодня).
    long daysOpen(const std::string& today) const;
    /// Привязать дефект к заказу-наряду.
    void linkOrder(RepairOrder& order);

    const std::string& getCode() const { return code_; }
    const std::string& getDescription() const { return description_; }
    int getSeverity() const { return severity_; }
    const std::string& getUnit() const { return unit_; }
    const std::string& getFoundAt() const { return foundAt_; }
    const std::string& getFixedAt() const { return fixedAt_; }
    RepairOrder* getOrder() const { return order_; }
    bool isFixed() const { return !fixedAt_.empty(); }

private:
    std::string code_;
    std::string description_;
    int severity_ = 1;
    std::string unit_;
    std::string foundAt_;
    std::string fixedAt_;
    RepairOrder* order_ = nullptr;
};

} // namespace autoservice

#endif
