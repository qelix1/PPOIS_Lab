#ifndef STUDENTIS_FINANCE_SCHOLARSHIP_H
#define STUDENTIS_FINANCE_SCHOLARSHIP_H

#include <string>

namespace studentis {

class AttendanceJournal;
class GradeBook;
class Student;

/**
 * @brief Стипендия: вид, размер, требования и лимит получателей.
 *
 * Виды: academic (нужен средний балл, отсутствие долгов и посещаемость) и
 * social (средний балл не учитывается, но ограничение пропусков действует).
 */
class Scholarship {
public:
    Scholarship() = default;
    Scholarship(std::string id, std::string name, std::string type, double amount,
                double minAverage = 0.0, int maxAbsences = 0, int budgetLimit = 0,
                bool active = true);

    /// Студент соответствует условиям стипендии.
    bool isEligible(const Student& student, const GradeBook& gradeBook,
                    const AttendanceJournal& journal) const;
    /// Размер стипендии.
    double amountFor() const;
    /// Стипендия социальная.
    bool isSocial() const;
    /// Стипендия академическая.
    bool isAcademic() const;
    /// Средний балл не ниже требуемого.
    bool requireAverage(double average) const;
    /// Отключить стипендию.
    void deactivate();

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getType() const { return type_; }
    double getAmount() const { return amount_; }
    double getMinAverage() const { return minAverage_; }
    int getMaxAbsences() const { return maxAbsences_; }
    int getBudgetLimit() const { return budgetLimit_; }
    bool isActive() const { return active_; }

    void setName(const std::string& name) { name_ = name; }
    void setAmount(double amount);
    void setMinAverage(double minAverage);
    void setMaxAbsences(int maxAbsences);
    void setBudgetLimit(int budgetLimit);

private:
    std::string id_;
    std::string name_;
    std::string type_;
    double amount_ = 0.0;
    double minAverage_ = 0.0;
    int maxAbsences_ = 0;
    int budgetLimit_ = 0;
    bool active_ = true;
};

} // namespace studentis

#endif
