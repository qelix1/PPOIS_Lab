#ifndef STUDENTIS_SERVICES_LIBRARYCARD_H
#define STUDENTIS_SERVICES_LIBRARYCARD_H

#include <string>
#include <vector>

namespace studentis {

class BookLoan;
class Student;

/**
 * @brief Читательский билет: срок действия, статус и выданные книги.
 *
 * Действителен при active студенте и issuedAt_ <= date <= expiresAt_.
 */
class LibraryCard {
public:
    LibraryCard() = default;
    LibraryCard(std::string id, Student* student, std::string issuedAt, std::string expiresAt,
                std::string status = "active");

    /// Зарегистрировать выдачу; недействительный билет — DocumentExpiredException.
    void addLoan(BookLoan& loan);
    /// Закрыть выдачу по номеру; неизвестная выдача — InvalidStudentDataException.
    void returnLoan(const std::string& loanId, const std::string& returnDate);
    /// Есть ли просроченные активные выдачи на указанную дату.
    bool hasOverdue(const std::string& date) const;
    /// Активные выдачи билета.
    std::vector<BookLoan*> activeLoans() const;
    /// Билет действителен на указанную дату.
    bool isValid(const std::string& date) const;
    /// Продлить билет до новой даты; дата не позже текущей — InvalidStudentDataException.
    void renew(const std::string& newExpiresAt);
    /// Суммарный штраф по активным выдачам на указанную дату.
    double debtFine(const std::string& date) const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    const std::string& getIssuedAt() const { return issuedAt_; }
    const std::string& getExpiresAt() const { return expiresAt_; }
    const std::string& getStatus() const { return status_; }
    const std::vector<BookLoan*>& getLoans() const { return loans_; }

    void setExpiresAt(const std::string& expiresAt) { expiresAt_ = expiresAt; }
    void setStatus(const std::string& status) { status_ = status; }

private:
    std::string id_;
    Student* student_ = nullptr;
    std::string issuedAt_;
    std::string expiresAt_;
    std::string status_;
    std::vector<BookLoan*> loans_;
};

} // namespace studentis

#endif
