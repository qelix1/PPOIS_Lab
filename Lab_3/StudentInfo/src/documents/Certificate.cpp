#include "studentis/documents/Certificate.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/DocumentExpiredException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownType(const std::string& type) {
    return type == "study" || type == "military" || type == "employment" || type == "bank" ||
           type == "other";
}

} // namespace

Certificate::Certificate(std::string id, Student* student, std::string type, std::string issuedAt,
                         Teacher* issuedBy, std::string purpose, std::string serialNumber,
                         std::string validUntil, std::string text)
    : id_(std::move(id)),
      student_(student),
      type_(std::move(type)),
      issuedAt_(std::move(issuedAt)),
      issuedBy_(issuedBy),
      purpose_(std::move(purpose)),
      serialNumber_(std::move(serialNumber)),
      validUntil_(std::move(validUntil)),
      text_(std::move(text)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор справки не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("справка должна относиться к студенту");
    if (!isKnownType(type_)) throw InvalidStudentDataException("неизвестный тип справки: " + type_);
    if (!date::isDate(issuedAt_)) throw InvalidStudentDataException("некорректная дата выдачи справки");
    if (!date::isDate(validUntil_)) {
        throw InvalidStudentDataException("некорректная дата окончания действия справки");
    }
    if (date::daysBetween(issuedAt_, validUntil_) < 0) {
        throw InvalidStudentDataException("срок действия справки раньше даты выдачи");
    }
    if (serialNumber_.empty()) throw InvalidStudentDataException("серия справки не может быть пустой");
}

bool Certificate::isValid(const std::string& date) const {
    if (student_ == nullptr || !student_->isActive() || validUntil_.empty()) return false;
    if (!date::isDate(date)) return false;
    return date::daysBetween(issuedAt_, date) >= 0 && date::daysBetween(date, validUntil_) >= 0;
}

std::string Certificate::generateText() {
    if (!isValid(date::today())) {
        throw DocumentExpiredException("справка " + serialNumber_ + " недействительна");
    }
    text_ = "Справка № " + serialNumber_ + " от " + issuedAt_ + " выдана " + student_->fullName() +
            " (" + type_ + ") для " + purpose_ + "; действует до " + validUntil_;
    if (issuedBy_ != nullptr) text_ += ". Подписал: " + issuedBy_->shortName();
    return text_;
}

bool Certificate::isForMilitary() const {
    return type_ == "military";
}

int Certificate::daysUntilExpiry(const std::string& date) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата проверки справки");
    if (validUntil_.empty()) return 0;
    return static_cast<int>(date::daysBetween(date, validUntil_));
}

void Certificate::revoke() {
    if (validUntil_.empty()) throw InvalidStudentDataException("справка уже отозвана");
    validUntil_.clear();
    text_ = "отозвана";
}

std::string Certificate::summary() const {
    std::string result = "Справка № " + serialNumber_ + " (" + type_ + ")";
    result += student_ != nullptr ? " для " + student_->shortName() : "";
    result += validUntil_.empty() ? ", отозвана" : ", действует до " + validUntil_;
    return result;
}

} // namespace studentis
