#include "studentis/services/LibraryCard.h"

#include <string>
#include <vector>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/DocumentExpiredException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/services/BookLoan.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "active" || status == "blocked";
}

} // namespace

LibraryCard::LibraryCard(std::string id, Student* student, std::string issuedAt,
                         std::string expiresAt, std::string status)
    : id_(std::move(id)),
      student_(student),
      issuedAt_(std::move(issuedAt)),
      expiresAt_(std::move(expiresAt)),
      status_(std::move(status)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор билета не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("билет должен относиться к студенту");
    if (!date::isDate(issuedAt_)) throw InvalidStudentDataException("некорректная дата выдачи билета");
    if (!date::isDate(expiresAt_)) throw InvalidStudentDataException("некорректная дата окончания билета");
    if (date::daysBetween(issuedAt_, expiresAt_) < 0) {
        throw InvalidStudentDataException("срок действия билета раньше даты выдачи");
    }
    if (!isKnownStatus(status_)) throw InvalidStudentDataException("неизвестный статус билета: " + status_);
}

void LibraryCard::addLoan(BookLoan& loan) {
    if (loan.getStudent() != student_) {
        throw InvalidStudentDataException("выдача оформлена на другого студента");
    }
    if (!isValid(loan.getBorrowedAt())) {
        throw DocumentExpiredException("читательский билет " + id_ + " недействителен");
    }
    for (const BookLoan* current : loans_) {
        if (current == &loan || current->getId() == loan.getId()) return;
    }
    loans_.push_back(&loan);
}

void LibraryCard::returnLoan(const std::string& loanId, const std::string& returnDate) {
    for (BookLoan* loan : loans_) {
        if (loan->getId() != loanId) continue;
        loan->returnBook(returnDate);
        return;
    }
    throw InvalidStudentDataException("выдача " + loanId + " не найдена в билете");
}

bool LibraryCard::hasOverdue(const std::string& date) const {
    for (const BookLoan* loan : loans_) {
        if (loan->isOverdue(date)) return true;
    }
    return false;
}

std::vector<BookLoan*> LibraryCard::activeLoans() const {
    std::vector<BookLoan*> result;
    for (BookLoan* loan : loans_) {
        if (loan->isActive()) result.push_back(loan);
    }
    return result;
}

bool LibraryCard::isValid(const std::string& date) const {
    if (student_ == nullptr || !student_->isActive() || status_ != "active") return false;
    if (!date::isDate(date)) return false;
    return date::daysBetween(issuedAt_, date) >= 0 && date::daysBetween(date, expiresAt_) >= 0;
}

void LibraryCard::renew(const std::string& newExpiresAt) {
    if (status_ != "active") throw DocumentExpiredException("заблокированный билет нельзя продлить");
    if (!date::isDate(newExpiresAt)) throw InvalidStudentDataException("некорректная новая дата билета");
    if (date::daysBetween(expiresAt_, newExpiresAt) <= 0) {
        throw InvalidStudentDataException("новая дата должна быть позже текущей");
    }
    expiresAt_ = newExpiresAt;
}

double LibraryCard::debtFine(const std::string& date) const {
    double total = 0.0;
    for (const BookLoan* loan : loans_) {
        total += loan->calculateFine(date);
    }
    return total;
}

} // namespace studentis
