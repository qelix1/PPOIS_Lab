#ifndef AUTOSERVICE_FINANCE_INVOICE_H
#define AUTOSERVICE_FINANCE_INVOICE_H

#include <string>
#include <vector>

namespace autoservice {

class InvoiceLine;
class Client;
class RepairOrder;
class Discount;

/**
 * @brief Счёт клиенту за ремонт: строки, оплата, скидка.
 *
 * Статусы: draft → issued → paid.
 */
class Invoice {
public:
    Invoice();
    Invoice(std::string number, std::string createdAt, std::string dueAt);

    /// Добавить строку счёта.
    void addLine(InvoiceLine& line);
    /// Пересчитать итог по строкам с учётом скидки.
    void recalcTotal();
    /// Учесть поступившую оплату.
    void markPaid(int amountRub);
    /// Счёт просрочен и не оплачен.
    bool isOverdue(const std::string& today) const;
    /// Применить скидку к счёту.
    void applyDiscount(const Discount& discount);
    /// Долг по счёту.
    int balance() const;
    /// Выставить счёт клиенту.
    void issue();

    const std::string& getNumber() const { return number_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getDueAt() const { return dueAt_; }
    const std::string& getStatus() const { return status_; }
    int getTotalRub() const { return totalRub_; }
    int getPaidRub() const { return paidRub_; }
    const std::vector<InvoiceLine*>& getLines() const { return lines_; }
    Client* getClient() const { return client_; }
    RepairOrder* getOrder() const { return order_; }
    const Discount* getDiscount() const { return discount_; }
    void setClient(Client& client) { client_ = &client; }
    void setOrder(RepairOrder& order) { order_ = &order; }

private:
    std::string number_;
    std::string createdAt_;
    std::string dueAt_;
    std::string status_ = "draft";
    std::vector<InvoiceLine*> lines_;
    int totalRub_ = 0;
    int paidRub_ = 0;
    Client* client_ = nullptr;
    RepairOrder* order_ = nullptr;
    const Discount* discount_ = nullptr;
};

} // namespace autoservice

#endif
