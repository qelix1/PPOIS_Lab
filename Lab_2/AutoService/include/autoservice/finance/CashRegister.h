#ifndef AUTOSERVICE_FINANCE_CASH_REGISTER_H
#define AUTOSERVICE_FINANCE_CASH_REGISTER_H

#include <string>
#include <vector>

namespace autoservice {

class FinancialTransaction;
class Payment;
class Workshop;

/**
 * @brief Касса мастерской: остаток, приход/расход, дневной итог.
 */
class CashRegister {
public:
    CashRegister();
    CashRegister(std::string id, std::string cashierName);

    /// Открыть кассу на дату.
    void open(const std::string& date);
    /// Закрыть кассу на дату.
    void close(const std::string& date);
    /// Положить деньги в кассу.
    void deposit(int rub);
    /// Забрать деньги из кассы (недостаток средств — PaymentFailedException).
    void withdraw(int rub);
    /// Записать финансовую операцию.
    void recordTransaction(FinancialTransaction& transaction);
    /// Итог за день по записанным операциям.
    int dayTotal() const;

    const std::string& getId() const { return id_; }
    const std::string& getCashierName() const { return cashierName_; }
    int getBalanceRub() const { return balanceRub_; }
    const std::string& getOpenedAt() const { return openedAt_; }
    const std::string& getClosedAt() const { return closedAt_; }
    const std::vector<FinancialTransaction*>& getTransactions() const { return transactions_; }
    Workshop* getWorkshop() const { return workshop_; }
    void setWorkshop(Workshop& workshop) { workshop_ = &workshop; }
    bool isOpen() const { return !openedAt_.empty() && closedAt_.empty(); }

private:
    std::string id_;
    int balanceRub_ = 0;
    std::string openedAt_;
    std::string closedAt_;
    std::string cashierName_;
    std::vector<FinancialTransaction*> transactions_;
    Workshop* workshop_ = nullptr;
};

} // namespace autoservice

#endif
