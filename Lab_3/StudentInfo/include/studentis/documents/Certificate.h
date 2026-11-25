#ifndef STUDENTIS_DOCUMENTS_CERTIFICATE_H
#define STUDENTIS_DOCUMENTS_CERTIFICATE_H

#include <string>

namespace studentis {

class Student;
class Teacher;

/**
 * @brief Справка студента: тип, цель, серия, срок действия и текст.
 *
 * Действительна при active студенте и issuedAt_ <= date <= validUntil_.
 * Отозванная справка имеет пустой validUntil_.
 */
class Certificate {
public:
    Certificate() = default;
    Certificate(std::string id, Student* student, std::string type, std::string issuedAt,
                Teacher* issuedBy, std::string purpose, std::string serialNumber,
                std::string validUntil, std::string text = "");

    /// Справка действительна на указанную дату.
    bool isValid(const std::string& date) const;
    /// Сформировать текст справки; недействительная справка — DocumentExpiredException.
    std::string generateText();
    /// Справка предназначена для военкомата.
    bool isForMilitary() const;
    /// Дней до окончания срока действия; у отозванной — 0.
    int daysUntilExpiry(const std::string& date) const;
    /// Отозвать справку.
    void revoke();
    /// Краткое текстовое описание справки.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    const std::string& getType() const { return type_; }
    const std::string& getIssuedAt() const { return issuedAt_; }
    Teacher* getIssuedBy() const { return issuedBy_; }
    const std::string& getPurpose() const { return purpose_; }
    const std::string& getSerialNumber() const { return serialNumber_; }
    const std::string& getValidUntil() const { return validUntil_; }
    const std::string& getText() const { return text_; }

    void setPurpose(const std::string& purpose) { purpose_ = purpose; }

private:
    std::string id_;
    Student* student_ = nullptr;
    std::string type_;
    std::string issuedAt_;
    Teacher* issuedBy_ = nullptr;
    std::string purpose_;
    std::string serialNumber_;
    std::string validUntil_;
    std::string text_;
};

} // namespace studentis

#endif
