#include "studentis/finance/Scholarship.h"

#include "studentis/assessment/AttendanceJournal.h"
#include "studentis/assessment/GradeBook.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InsufficientFundsException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownType(const std::string& type) {
    return type == "academic" || type == "social";
}

} // namespace

Scholarship::Scholarship(std::string id, std::string name, std::string type, double amount,
                         double minAverage, int maxAbsences, int budgetLimit, bool active)
    : id_(std::move(id)),
      name_(std::move(name)),
      type_(std::move(type)),
      amount_(amount),
      minAverage_(minAverage),
      maxAbsences_(maxAbsences),
      budgetLimit_(budgetLimit),
      active_(active) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор стипендии не может быть пустым");
    if (name_.empty()) throw InvalidStudentDataException("название стипендии не может быть пустым");
    if (!isKnownType(type_)) throw InvalidStudentDataException("неизвестный вид стипендии: " + type_);
    setAmount(amount);
    setMinAverage(minAverage);
    setMaxAbsences(maxAbsences);
    setBudgetLimit(budgetLimit);
}

bool Scholarship::isEligible(const Student& student, const GradeBook& gradeBook,
                             const AttendanceJournal& journal) const {
    if (!active_ || !student.isActive()) return false;
    if (journal.unexcusedAbsences(student) > maxAbsences_) return false;
    if (isAcademic()) {
        if (gradeBook.getStudent() != &student) return false;
        if (gradeBook.average() < minAverage_) return false;
        if (gradeBook.hasDebts()) return false;
    }
    return true;
}

double Scholarship::amountFor() const {
    return amount_;
}

bool Scholarship::isSocial() const {
    return type_ == "social";
}

bool Scholarship::isAcademic() const {
    return type_ == "academic";
}

bool Scholarship::requireAverage(double average) const {
    return average >= minAverage_;
}

void Scholarship::deactivate() {
    active_ = false;
}

void Scholarship::setAmount(double amount) {
    if (amount <= 0.0) throw InsufficientFundsException("размер стипендии должен быть положительным");
    amount_ = amount;
}

void Scholarship::setMinAverage(double minAverage) {
    if (minAverage < 0.0 || minAverage > 100.0) {
        throw InvalidStudentDataException("минимальный средний балл должен быть от 0 до 100");
    }
    minAverage_ = minAverage;
}

void Scholarship::setMaxAbsences(int maxAbsences) {
    if (maxAbsences < 0) {
        throw InvalidStudentDataException("допустимое число пропусков не может быть отрицательным");
    }
    maxAbsences_ = maxAbsences;
}

void Scholarship::setBudgetLimit(int budgetLimit) {
    if (budgetLimit < 0) {
        throw InvalidStudentDataException("лимит получателей не может быть отрицательным");
    }
    budgetLimit_ = budgetLimit;
}

} // namespace studentis
