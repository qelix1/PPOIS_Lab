#ifndef STUDENTIS_DOCUMENTS_STUDENTCARD_H
#define STUDENTIS_DOCUMENTS_STUDENTCARD_H

#include <string>

namespace studentis {

class Student;

/**
 * @brief Студенческий пропуск: срок действия, статус, PIN и уровень доступа.
 *
 * Действителен, когда статус active и issuedAt_ <= date <= expiresAt_.
 */
class StudentCard {
public:
    StudentCard() = default;
    StudentCard(std::string number, Student* student, std::string issuedAt, std::string expiresAt,
                std::string status, std::string pinHash, std::string accessLevel);

    /// Активировать пропуск.
    void activate();
    /// Заблокировать пропуск.
    void block();
    /// Продлить пропуск на новый срок и активировать его.
    void renew(const std::string& issuedAt, const std::string& expiresAt);
    /// Пропуск действителен на указанную дату.
    bool isValid(const std::string& date) const;
    /// Проверить PIN; недействительный пропуск — DocumentExpiredException.
    bool verifyPin(const std::string& pin) const;
    /// Достаточен ли уровень доступа пропуска для указанного уровня.
    bool canEnter(const std::string& requiredLevel, const std::string& date) const;
    /// Дней до истечения срока действия (отрицательное значение — просрочен).
    int daysUntilExpiry(const std::string& date) const;

    const std::string& getNumber() const { return number_; }
    Student* getStudent() const { return student_; }
    const std::string& getIssuedAt() const { return issuedAt_; }
    const std::string& getExpiresAt() const { return expiresAt_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getPinHash() const { return pinHash_; }
    const std::string& getAccessLevel() const { return accessLevel_; }

    void setStatus(const std::string& status);
    void setPinHash(const std::string& pinHash) { pinHash_ = pinHash; }
    void setAccessLevel(const std::string& accessLevel);

private:
    std::string number_;
    Student* student_ = nullptr;
    std::string issuedAt_;
    std::string expiresAt_;
    std::string status_ = "active";
    std::string pinHash_;
    std::string accessLevel_ = "basic";
};

} // namespace studentis

#endif
