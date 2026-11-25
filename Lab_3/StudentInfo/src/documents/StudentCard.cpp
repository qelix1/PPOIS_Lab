#include "studentis/documents/StudentCard.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/DocumentExpiredException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "active" || status == "blocked" || status == "expired";
}

int levelRank(const std::string& level) {
    if (level == "basic") return 1;
    if (level == "extended") return 2;
    if (level == "full") return 3;
    return 0;
}

bool isKnownLevel(const std::string& level) {
    return levelRank(level) > 0;
}

} // namespace

StudentCard::StudentCard(std::string number, Student* student, std::string issuedAt,
                         std::string expiresAt, std::string status, std::string pinHash,
                         std::string accessLevel)
    : number_(std::move(number)),
      student_(student),
      issuedAt_(std::move(issuedAt)),
      expiresAt_(std::move(expiresAt)),
      pinHash_(std::move(pinHash)),
      accessLevel_(std::move(accessLevel)) {
    if (number_.empty()) throw InvalidStudentDataException("номер пропуска не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("пропуск должен быть привязан к студенту");
    if (!date::isDate(issuedAt_)) throw InvalidStudentDataException("некорректная дата выдачи пропуска");
    if (!date::isDate(expiresAt_)) {
        throw InvalidStudentDataException("некорректная дата окончания пропуска");
    }
    if (date::daysBetween(issuedAt_, expiresAt_) < 0) {
        throw InvalidStudentDataException("срок действия пропуска раньше даты выдачи");
    }
    if (pinHash_.empty()) throw InvalidStudentDataException("PIN пропуска не может быть пустым");
    setStatus(status);
    setAccessLevel(accessLevel_);
}

void StudentCard::activate() {
    status_ = "active";
}

void StudentCard::block() {
    status_ = "blocked";
}

void StudentCard::renew(const std::string& issuedAt, const std::string& expiresAt) {
    if (!date::isDate(issuedAt) || !date::isDate(expiresAt)) {
        throw InvalidStudentDataException("некорректные даты продления пропуска");
    }
    if (date::daysBetween(issuedAt, expiresAt) < 0) {
        throw InvalidStudentDataException("срок действия пропуска раньше даты выдачи");
    }
    issuedAt_ = issuedAt;
    expiresAt_ = expiresAt;
    status_ = "active";
}

bool StudentCard::isValid(const std::string& date) const {
    if (status_ != "active" || !date::isDate(date)) return false;
    return date::daysBetween(issuedAt_, date) >= 0 && date::daysBetween(date, expiresAt_) >= 0;
}

bool StudentCard::verifyPin(const std::string& pin) const {
    if (!isValid(date::today())) {
        throw DocumentExpiredException("пропуск " + number_ + " недействителен");
    }
    return pinHash_ == pin;
}

bool StudentCard::canEnter(const std::string& requiredLevel, const std::string& date) const {
    if (!isKnownLevel(requiredLevel)) {
        throw InvalidStudentDataException("неизвестный уровень доступа: " + requiredLevel);
    }
    if (!isValid(date)) return false;
    return levelRank(accessLevel_) >= levelRank(requiredLevel);
}

int StudentCard::daysUntilExpiry(const std::string& date) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата проверки пропуска");
    return static_cast<int>(date::daysBetween(date, expiresAt_));
}

void StudentCard::setStatus(const std::string& status) {
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус пропуска: " + status);
    status_ = status;
}

void StudentCard::setAccessLevel(const std::string& accessLevel) {
    if (!isKnownLevel(accessLevel)) {
        throw InvalidStudentDataException("неизвестный уровень доступа: " + accessLevel);
    }
    accessLevel_ = accessLevel;
}

} // namespace studentis
