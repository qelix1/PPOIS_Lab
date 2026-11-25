#ifndef STUDENTIS_FINANCE_TUITIONPAYMENT_H
#define STUDENTIS_FINANCE_TUITIONPAYMENT_H

#include <string>

namespace studentis {

class Teacher;
class TuitionContract;

/**
 * @brief Платёж по договору об обучении: сумма, дата, способ и подтверждение.
 *
 * Статусы: pending, confirmed, cancelled. Платёж считается просроченным,
 * если внесён позже даты начала договора.
 */
class TuitionPayment {
public:
    TuitionPayment() = default;
    TuitionPayment(std::string id, TuitionContract* contract, double amount, std::string paidAt,
                   std::string method, std::string receiptNumber,
                   Teacher* confirmedBy = nullptr, std::string status = "pending");

    /// Подтвердить платёж преподавателем; отменённый платёж — ошибка данных.
    void confirm(Teacher& teacher);
    /// Отменить неподтверждённый платёж; подтверждённый — ошибка данных.
    void cancel();
    /// Платёж подтверждён.
    bool isConfirmed() const;
    /// Платёж внесён позже начала договора.
    bool isLate() const;
    /// Дата платежа попадает в период [startDate, endDate].
    bool matchesPeriod(const std::string& startDate, const std::string& endDate) const;
    /// Текст квитанции.
    std::string receipt() const;
    /// Краткое текстовое описание платежа.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    TuitionContract* getContract() const { return contract_; }
    double getAmount() const { return amount_; }
    const std::string& getPaidAt() const { return paidAt_; }
    const std::string& getMethod() const { return method_; }
    const std::string& getReceiptNumber() const { return receiptNumber_; }
    Teacher* getConfirmedBy() const { return confirmedBy_; }
    const std::string& getStatus() const { return status_; }

    void setContract(TuitionContract* contract) { contract_ = contract; }
    void setMethod(const std::string& method);
    void setReceiptNumber(const std::string& receiptNumber);

private:
    std::string id_;
    TuitionContract* contract_ = nullptr;
    double amount_ = 0.0;
    std::string paidAt_;
    std::string method_;
    std::string receiptNumber_;
    Teacher* confirmedBy_ = nullptr;
    std::string status_ = "pending";
};

} // namespace studentis

#endif
