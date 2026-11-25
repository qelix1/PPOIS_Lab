#include "studentis/services/BookLoan.h"

#include <string>

#include "studentis/core/DateUtils.h"
#include "studentis/services/Book.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "active" || status == "returned";
}

} // namespace

BookLoan::BookLoan(std::string id, Book* book, Student* student, std::string borrowedAt,
                   std::string dueDate, double finePerDay, std::string returnedAt,
                   std::string status)
    : id_(std::move(id)),
      book_(book),
      student_(student),
      borrowedAt_(std::move(borrowedAt)),
      dueDate_(std::move(dueDate)),
      returnedAt_(std::move(returnedAt)),
      finePerDay_(finePerDay),
      status_(std::move(status)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор выдачи не может быть пустым");
    if (book_ == nullptr) throw InvalidStudentDataException("выдача должна относиться к книге");
    if (student_ == nullptr) throw InvalidStudentDataException("выдача должна относиться к студенту");
    if (!date::isDate(borrowedAt_)) throw InvalidStudentDataException("некорректная дата выдачи книги");
    if (!date::isDate(dueDate_)) throw InvalidStudentDataException("некорректная дата возврата книги");
    if (date::daysBetween(borrowedAt_, dueDate_) < 0) {
        throw InvalidStudentDataException("срок возврата раньше даты выдачи");
    }
    if (!returnedAt_.empty() && !date::isDate(returnedAt_)) {
        throw InvalidStudentDataException("некорректная дата фактического возврата");
    }
    if (finePerDay_ < 0.0) throw InvalidStudentDataException("штраф за день не может быть отрицательным");
    if (!isKnownStatus(status_)) throw InvalidStudentDataException("неизвестный статус выдачи: " + status_);
}

bool BookLoan::isOverdue(const std::string& date) const {
    return overdueDays(date) > 0;
}

int BookLoan::overdueDays(const std::string& date) const {
    if (!isActive() || !date::isDate(date)) return 0;
    const long days = date::daysBetween(dueDate_, date);
    return days > 0 ? static_cast<int>(days) : 0;
}

double BookLoan::calculateFine(const std::string& date) const {
    return static_cast<double>(overdueDays(date)) * finePerDay_;
}

void BookLoan::returnBook(const std::string& returnDate) {
    if (!isActive()) throw InvalidStudentDataException("выдача " + id_ + " уже закрыта");
    if (!date::isDate(returnDate)) throw InvalidStudentDataException("некорректная дата возврата");
    if (date::daysBetween(borrowedAt_, returnDate) < 0) {
        throw InvalidStudentDataException("дата возврата раньше даты выдачи");
    }
    returnedAt_ = returnDate;
    status_ = "returned";
    if (book_ != nullptr) book_->returnCopy();
}

void BookLoan::extend(const std::string& newDueDate) {
    if (!isActive()) throw InvalidStudentDataException("продлить можно только активную выдачу");
    if (!date::isDate(newDueDate)) throw InvalidStudentDataException("некорректная новая дата возврата");
    if (date::daysBetween(dueDate_, newDueDate) <= 0) {
        throw InvalidStudentDataException("новая дата возврата должна быть позже текущей");
    }
    dueDate_ = newDueDate;
}

bool BookLoan::isActive() const {
    return status_ == "active";
}

} // namespace studentis
