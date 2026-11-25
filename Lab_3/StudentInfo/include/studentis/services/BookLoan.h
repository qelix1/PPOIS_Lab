#ifndef STUDENTIS_SERVICES_BOOKLOAN_H
#define STUDENTIS_SERVICES_BOOKLOAN_H

#include <string>

namespace studentis {

class Book;
class Student;

/**
 * @brief Выдача книги: срок возврата, штраф за день просрочки и статус.
 *
 * Статусы: active, returned.
 */
class BookLoan {
public:
    BookLoan() = default;
    BookLoan(std::string id, Book* book, Student* student, std::string borrowedAt,
             std::string dueDate, double finePerDay, std::string returnedAt = "",
             std::string status = "active");

    /// Выдача просрочена на указанную дату.
    bool isOverdue(const std::string& date) const;
    /// Число дней просрочки на указанную дату (0 для возвращённой книги).
    int overdueDays(const std::string& date) const;
    /// Штраф за просрочку: дни просрочки × штраф за день.
    double calculateFine(const std::string& date) const;
    /// Вернуть книгу: дата возврата, статус returned и плюс один свободный экземпляр.
    void returnBook(const std::string& returnDate);
    /// Продлить выдачу; новая дата не позже текущей — InvalidStudentDataException.
    void extend(const std::string& newDueDate);
    /// Выдача ещё не закрыта.
    bool isActive() const;

    const std::string& getId() const { return id_; }
    Book* getBook() const { return book_; }
    Student* getStudent() const { return student_; }
    const std::string& getBorrowedAt() const { return borrowedAt_; }
    const std::string& getDueDate() const { return dueDate_; }
    const std::string& getReturnedAt() const { return returnedAt_; }
    double getFinePerDay() const { return finePerDay_; }
    const std::string& getStatus() const { return status_; }

    void setDueDate(const std::string& dueDate) { dueDate_ = dueDate; }
    void setStatus(const std::string& status) { status_ = status; }

private:
    std::string id_;
    Book* book_ = nullptr;
    Student* student_ = nullptr;
    std::string borrowedAt_;
    std::string dueDate_;
    std::string returnedAt_;
    double finePerDay_ = 0.0;
    std::string status_ = "active";
};

} // namespace studentis

#endif
