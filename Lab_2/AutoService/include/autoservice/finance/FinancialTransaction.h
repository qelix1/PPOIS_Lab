#ifndef AUTOSERVICE_FINANCE_FINANCIAL_TRANSACTION_H
#define AUTOSERVICE_FINANCE_FINANCIAL_TRANSACTION_H

#include <string>

namespace autoservice {

class CashRegister;
class Invoice;

/**
 * @brief Финансовая операция кассы: приход или расход.
 */
class FinancialTransaction {
public:
    FinancialTransaction();
    FinancialTransaction(std::string id, std::string type, int amountRub, std::string createdAt);

    /// Операция приходная.
    bool isIncoming() const;
    /// Реверс операции: приход ↔ расход.
    void reverse(const std::string& reason);
    /// Подпись операции для отчёта.
    std::string describe() const;
    /// Сумма со знаком (+ приход, − расход).
    int signedAmount() const;

    const std::string& getId() const { return id_; }
    const std::string& getType() const { return type_; }
    int getAmountRub() const { return amountRub_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getComment() const { return comment_; }
    CashRegister* getRegister() const { return register_; }
    Invoice* getInvoice() const { return invoice_; }
    void setRegister(CashRegister& reg) { register_ = &reg; }
    void setInvoice(Invoice& invoice) { invoice_ = &invoice; }
    void setComment(const std::string& comment) { comment_ = comment; }

private:
    std::string id_;
    std::string type_;
    int amountRub_ = 0;
    std::string createdAt_;
    std::string comment_;
    CashRegister* register_ = nullptr;
    Invoice* invoice_ = nullptr;
};

} // namespace autoservice

#endif
