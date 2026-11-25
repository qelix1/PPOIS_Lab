#ifndef STUDENTIS_FINANCE_TUITIONCONTRACT_H
#define STUDENTIS_FINANCE_TUITIONCONTRACT_H

#include <string>

namespace studentis {

class Specialty;
class Student;

/**
 * @brief Договор об оплате обучения: срок, годовая сумма и скидка.
 *
 * Годовая стоимость считается с учётом скидки, semesterAmount() — её половина.
 * Статусы: draft, active, terminated.
 */
class TuitionContract {
public:
    TuitionContract() = default;
    TuitionContract(std::string id, Student* student, Specialty* specialty, std::string signedAt,
                    std::string startDate, std::string endDate, double annualAmount,
                    double discountPercent = 0.0, std::string status = "active");

    /// Стоимость года с учётом скидки.
    double yearlyTotal() const;
    /// Стоимость половины года (семестра).
    double semesterAmount() const;
    /// Договор действует (статус active).
    bool isActive() const;
    /// Применить скидку; вне 0..100 — ошибка данных.
    void applyDiscount(double percent);
    /// Расторгнуть договор.
    void terminate();
    /// Дней до окончания договора на указанную дату (отрицательное — просрочен).
    long daysUntilExpiry(const std::string& date) const;
    /// Неоплаченная стоимость оставшегося срока договора на указанную дату.
    double remainingAmount(const std::string& date) const;
    /// Даты договора корректны и начало не позже окончания.
    bool validateDates() const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    Specialty* getSpecialty() const { return specialty_; }
    const std::string& getSignedAt() const { return signedAt_; }
    const std::string& getStartDate() const { return startDate_; }
    const std::string& getEndDate() const { return endDate_; }
    double getAnnualAmount() const { return annualAmount_; }
    double getDiscountPercent() const { return discountPercent_; }
    const std::string& getStatus() const { return status_; }

    void setSignedAt(const std::string& signedAt);
    void setStartDate(const std::string& startDate);
    void setEndDate(const std::string& endDate);
    void setAnnualAmount(double annualAmount);
    void setStatus(const std::string& status);

private:
    std::string id_;
    Student* student_ = nullptr;
    Specialty* specialty_ = nullptr;
    std::string signedAt_;
    std::string startDate_;
    std::string endDate_;
    double annualAmount_ = 0.0;
    double discountPercent_ = 0.0;
    std::string status_ = "active";
};

} // namespace studentis

#endif
