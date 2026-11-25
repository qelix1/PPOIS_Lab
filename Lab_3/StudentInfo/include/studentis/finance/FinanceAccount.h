#ifndef STUDENTIS_FINANCE_FINANCEACCOUNT_H
#define STUDENTIS_FINANCE_FINANCEACCOUNT_H

#include <string>
#include <vector>

namespace studentis {

class ScholarshipAward;
class Student;
class TuitionPayment;

/**
 * @brief Финансовый счёт студента: начисления, подтверждённые платежи и долг.
 *
 * balance_ знаковый: начисление вычитается, подтверждённый платёж прибавляется.
 * Долг равен max(0, -balance_).
 */
class FinanceAccount {
public:
    FinanceAccount() = default;
    explicit FinanceAccount(Student* student);

    /// Начислить сумму; неположительная сумма — InsufficientFundsException.
    void charge(double amount);
    /// Зарегистрировать подтверждённый платёж; переплата — InsufficientFundsException.
    void registerPayment(TuitionPayment& payment);
    /// Можно ли принять платёж указанного размера.
    bool canPay(double amount) const;
    /// Текущий долг (не меньше нуля).
    double debt() const;
    /// Платежи, внесённые в период [startDate, endDate].
    std::vector<TuitionPayment*> historyBetween(const std::string& startDate,
                                                const std::string& endDate) const;
    /// Есть ли непогашенный долг.
    bool hasDebt() const;
    /// Зачислить выплаченную стипендию на счёт.
    void applyScholarship(ScholarshipAward& award);

    Student* getStudent() const { return student_; }
    double getBalance() const { return balance_; }
    const std::vector<TuitionPayment*>& getTransactions() const { return transactions_; }

private:
    Student* student_ = nullptr;
    double balance_ = 0.0;
    std::vector<TuitionPayment*> transactions_;
};

} // namespace studentis

#endif
