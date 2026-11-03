#ifndef AUTOSERVICE_ORDERS_QUALITY_CHECK_H
#define AUTOSERVICE_ORDERS_QUALITY_CHECK_H

#include <string>
#include <vector>

namespace autoservice {

class Mechanic;
class RepairOrder;

/**
 * @brief Контроль качества (ОТК) выполненного ремонта.
 */
class QualityCheck {
public:
    QualityCheck();
    QualityCheck(std::string id, std::string checkedAt, int mileageAtCheck);

    /// Признать ремонт принятым.
    void pass();
    /// Признать ремонт с провалом с указанием причины.
    void fail(const std::string& reason);
    /// Зафиксировать замечание контроля качества.
    void addFinding(const std::string& item);
    /// Есть ли блокирующие замечания (провал или список не пуст).
    bool hasBlockingIssues() const;
    /// Текст отчёта ОТК (заглушка — не реализован).
    std::string report() const;

    const std::string& getId() const { return id_; }
    const std::string& getCheckedAt() const { return checkedAt_; }
    const std::string& getVerdict() const { return verdict_; }
    Mechanic* getInspector() const { return inspector_; }
    RepairOrder* getOrder() const { return order_; }
    const std::vector<std::string>& getFindings() const { return findings_; }
    int getMileageAtCheck() const { return mileageAtCheck_; }
    const std::string& getNextCheckAt() const { return nextCheckAt_; }
    void setInspector(Mechanic& inspector) { inspector_ = &inspector; }
    void setOrder(RepairOrder& order) { order_ = &order; }
    void setNextCheckAt(const std::string& date) { nextCheckAt_ = date; }

private:
    std::string id_;
    std::string checkedAt_;
    std::string verdict_;
    Mechanic* inspector_ = nullptr;
    RepairOrder* order_ = nullptr;
    std::vector<std::string> findings_;
    int mileageAtCheck_ = 0;
    std::string nextCheckAt_;
};

} // namespace autoservice

#endif
