#ifndef STUDENTIS_FINANCE_STUDENTFINE_H
#define STUDENTIS_FINANCE_STUDENTFINE_H

#include <string>

namespace studentis {

class Student;

/**
 * @brief Штраф студента: сумма, срок, частичные выплаты и списание.
 *
 * Остаток считается как max(0, amount - paidAmount). Статусы: issued, paid, waived.
 */
class StudentFine {
public:
    StudentFine() = default;
    StudentFine(std::string id, Student* student, std::string reason, double amount,
                std::string issuedAt, std::string dueDate, double paidAmount = 0.0,
                std::string status = "issued");

    /// Внести часть штрафа; переплата — InsufficientFundsException.
    void addPayment(double amount);
    /// Непогашенный остаток штрафа.
    double remaining() const;
    /// Штраф не погашен и срок оплаты уже прошёл.
    bool isOverdue(const std::string& date) const;
    /// Штраф погашен полностью.
    bool isPaid() const;
    /// Списать штраф без оплаты.
    void waive();
    /// Краткое текстовое описание штрафа.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    const std::string& getReason() const { return reason_; }
    double getAmount() const { return amount_; }
    const std::string& getIssuedAt() const { return issuedAt_; }
    const std::string& getDueDate() const { return dueDate_; }
    double getPaidAmount() const { return paidAmount_; }
    const std::string& getStatus() const { return status_; }

private:
    std::string id_;
    Student* student_ = nullptr;
    std::string reason_;
    double amount_ = 0.0;
    std::string issuedAt_;
    std::string dueDate_;
    double paidAmount_ = 0.0;
    std::string status_ = "issued";
};

} // namespace studentis

#endif
