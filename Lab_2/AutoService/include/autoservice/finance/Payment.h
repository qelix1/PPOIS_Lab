#ifndef AUTOSERVICE_FINANCE_PAYMENT_H
#define AUTOSERVICE_FINANCE_PAYMENT_H

#include <string>

namespace autoservice {

class Invoice;
class Client;
class CashRegister;
class Refund;

/**
 * @brief Платёж по счёту: способ оплата, касса, статус операции.
 *
 * Статусы: draft → authorized → completed | cancelled.
 */
class Payment {
public:
    Payment();
    Payment(std::string id, int amountRub, std::string method, std::string paidAt);

    /// Авторизовать платёж по счёту.
    void authorize();
    /// Провести платёж через кассу.
    void confirm();
    /// Отменить платёж.
    void cancel();
    /// Кассовый чек одной строкой.
    std::string receipt() const;
    /// Доступен ли возврат по сроку (30 дней от оплаты).
    bool isRefundable(const std::string& today) const;

    const std::string& getId() const { return id_; }
    int getAmountRub() const { return amountRub_; }
    const std::string& getMethod() const { return method_; }
    const std::string& getPaidAt() const { return paidAt_; }
    const std::string& getStatus() const { return status_; }
    Invoice* getInvoice() const { return invoice_; }
    Client* getClient() const { return client_; }
    CashRegister* getCashier() const { return cashier_; }
    void setInvoice(Invoice& invoice) { invoice_ = &invoice; }
    void setClient(Client& client) { client_ = &client; }
    void setCashier(CashRegister& cashier) { cashier_ = &cashier; }

private:
    std::string id_;
    int amountRub_ = 0;
    std::string method_;
    std::string paidAt_;
    std::string status_ = "draft";
    Invoice* invoice_ = nullptr;
    Client* client_ = nullptr;
    CashRegister* cashier_ = nullptr;
};

} // namespace autoservice

#endif
