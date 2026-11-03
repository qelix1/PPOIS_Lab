#ifndef AUTOSERVICE_ORDERS_ESTIMATE_H
#define AUTOSERVICE_ORDERS_ESTIMATE_H

#include <string>
#include <vector>

namespace autoservice {

class EstimateLine;
class Discount;
class Manager;
class RepairOrder;

/**
 * @brief Смета на ремонт: строки работ и запчастей, скидка, утверждение.
 */
class Estimate {
public:
    Estimate();
    Estimate(std::string number, std::string createdAt);

    /// Добавить строку сметы.
    void addLine(EstimateLine& line);
    /// Убрать строку сметы.
    void removeLine(const EstimateLine& line);
    /// Пересчитать итог с учётом строк и скидки.
    void recalcTotal();
    /// Утвердить смету менеджером.
    void approve(Manager& manager);
    /// Отклонить смету.
    void reject();
    /// Применить скидку (должна быть действительна сегодня).
    void applyDiscount(const Discount& discount);

    const std::string& getNumber() const { return number_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getStatus() const { return status_; }
    int getTotalRub() const { return totalRub_; }
    const std::vector<EstimateLine*>& getLines() const { return lines_; }
    const Discount* getDiscount() const { return discount_; }
    Manager* getApprovedBy() const { return approvedBy_; }
    void setOrder(RepairOrder& order) { order_ = &order; }

private:
    std::string number_;
    std::string createdAt_;
    std::string status_ = "draft";
    std::vector<EstimateLine*> lines_;
    const Discount* discount_ = nullptr;
    int totalRub_ = 0;
    Manager* approvedBy_ = nullptr;
    RepairOrder* order_ = nullptr;
};

} // namespace autoservice

#endif
