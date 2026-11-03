#ifndef AUTOSERVICE_FINANCE_REFUND_H
#define AUTOSERVICE_FINANCE_REFUND_H

#include <string>

namespace autoservice {

class Payment;
class Manager;
class CashRegister;
class Invoice;

/**
 * @brief Возврат денег клиенту по оплаченному счёту.
 *
 * Статусы: draft → approved → executed | rejected.
 */
class Refund {
public:
    Refund();
    Refund(std::string id, int amountRub, std::string reason, std::string createdAt);

    /// Согласовать возврат менеджером.
    void approve(Manager& manager);
    /// Отклонить возврат.
    void reject(const std::string& reason);
    /// Возврат ожидает решения.
    bool isPending() const;
    /// Выполнить возврат через кассу.
    void execute(CashRegister& cashier);

    const std::string& getId() const { return id_; }
    int getAmountRub() const { return amountRub_; }
    const std::string& getReason() const { return reason_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getStatus() const { return status_; }
    Payment* getPayment() const { return payment_; }
    Manager* getApprovedBy() const { return approvedBy_; }
    void setPayment(Payment& payment) { payment_ = &payment; }
    void setInvoice(Invoice& invoice) { invoice_ = &invoice; }

private:
    std::string id_;
    int amountRub_ = 0;
    std::string reason_;
    std::string createdAt_;
    std::string status_ = "draft";
    Payment* payment_ = nullptr;
    Manager* approvedBy_ = nullptr;
    Invoice* invoice_ = nullptr;
};

} // namespace autoservice

#endif
