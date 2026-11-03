#ifndef AUTOSERVICE_WORKSHOP_SHOP_REPORT_H
#define AUTOSERVICE_WORKSHOP_SHOP_REPORT_H

#include <string>
#include <vector>

namespace autoservice {

class Workshop;
class ClientFeedback;
class FinancialTransaction;

/**
 * @brief Операционный отчёт мастерской за период: выручка, заказы, рейтинг.
 */
class ShopReport {
public:
    ShopReport();
    ShopReport(std::string id, std::string periodStart, std::string periodEnd);

    /// Учесть выполненный заказ с суммой выручки.
    void addOrder(int revenueRub);
    /// Добавить отзыв клиента в отчёт.
    void addFeedback(ClientFeedback& feedback);
    /// Пересчитать средний рейтинг по отзывам.
    void recalcRating();
    /// Рентабельность в процентах при заданных расходах.
    double profitMargin(int expensesRub) const;
    /// Подписать отчёт.
    void markSigned(const std::string& by);
    /// Текст сводки (заглушка — не реализован).
    std::string summary() const;

    const std::string& getId() const { return id_; }
    const std::string& getPeriodStart() const { return periodStart_; }
    const std::string& getPeriodEnd() const { return periodEnd_; }
    int getRevenueRub() const { return revenueRub_; }
    int getOrdersCompleted() const { return ordersCompleted_; }
    double getRatingAvg() const { return ratingAvg_; }
    const std::string& getSignedBy() const { return signedBy_; }
    Workshop* getWorkshop() const { return workshop_; }
    const std::vector<ClientFeedback*>& getFeedbacks() const { return feedbacks_; }
    void setWorkshop(Workshop& workshop) { workshop_ = &workshop; }

private:
    std::string id_;
    std::string periodStart_;
    std::string periodEnd_;
    int revenueRub_ = 0;
    int ordersCompleted_ = 0;
    double ratingAvg_ = 0.0;
    std::string signedBy_;
    Workshop* workshop_ = nullptr;
    std::vector<ClientFeedback*> feedbacks_;
};

} // namespace autoservice

#endif
