#include "studentis/core/Person.h"

#include <cctype>

#include "studentis/core/DateUtils.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

/// Первый символ UTF-8 строки (для инициалов).
std::string firstCharacter(const std::string& value) {
    if (value.empty()) return "";
    const unsigned char first = static_cast<unsigned char>(value[0]);
    std::size_t length = 1;
    if ((first & 0xE0) == 0xC0) {
        length = 2;
    } else if ((first & 0xF0) == 0xE0) {
        length = 3;
    } else if ((first & 0xF8) == 0xF0) {
        length = 4;
    }
    return value.substr(0, length);
}

/// Нижний регистр для латиницы и кириллицы (UTF-8).
std::string lowered(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    for (std::size_t i = 0; i < value.size();) {
        const unsigned char first = static_cast<unsigned char>(value[i]);
        if (first == 0xD0 && i + 1 < value.size()) {
            const unsigned char second = static_cast<unsigned char>(value[i + 1]);
            if (second >= 0x90 && second <= 0x9F) {          // А..П
                result += static_cast<char>(0xD0);
                result += static_cast<char>(second + 0x20);
            } else if (second >= 0xA0 && second <= 0xAF) {   // Р..Я
                result += static_cast<char>(0xD1);
                result += static_cast<char>(second - 0x20);
            } else if (second == 0x81) {                     // Ё
                result += static_cast<char>(0xD1);
                result += static_cast<char>(0x91);
            } else {
                result += value[i];
                result += value[i + 1];
            }
            i += 2;
            continue;
        }
        result += static_cast<char>(std::tolower(first));
        ++i;
    }
    return result;
}

struct DateParts {
    int year;
    int month;
    int day;
};

DateParts partsOf(const std::string& iso) {
    return {std::stoi(iso.substr(0, 4)), std::stoi(iso.substr(5, 2)), std::stoi(iso.substr(8, 2))};
}

} // namespace

Person::Person(std::string id, std::string lastName, std::string firstName, std::string middleName,
               std::string birthDate, std::string gender, std::string phone, std::string email,
               std::string address)
    : id_(std::move(id)),
      lastName_(std::move(lastName)),
      firstName_(std::move(firstName)),
      middleName_(std::move(middleName)),
      birthDate_(std::move(birthDate)),
      gender_(std::move(gender)),
      phone_(std::move(phone)),
      email_(std::move(email)),
      address_(std::move(address)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор человека не может быть пустым");
    if (lastName_.empty() || firstName_.empty()) {
        throw InvalidStudentDataException("фамилия и имя человека обязательны");
    }
    if (!date::isDate(birthDate_)) {
        throw InvalidStudentDataException("некорректная дата рождения: " + birthDate_);
    }
}

std::string Person::fullName() const {
    std::string result = lastName_ + " " + firstName_;
    if (!middleName_.empty()) result += " " + middleName_;
    return result;
}

std::string Person::shortName() const {
    std::string result = lastName_ + " ";
    if (!firstName_.empty()) result += firstCharacter(firstName_) + ".";
    if (!middleName_.empty()) result += firstCharacter(middleName_) + ".";
    return result;
}

int Person::ageOn(const std::string& date) const {
    if (!date::isDate(birthDate_)) {
        throw InvalidStudentDataException("некорректная дата рождения: " + birthDate_);
    }
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата: " + date);
    const DateParts birth = partsOf(birthDate_);
    const DateParts on = partsOf(date);
    int age = on.year - birth.year;
    if (on.month < birth.month || (on.month == birth.month && on.day < birth.day)) --age;
    return age;
}

bool Person::isAdultOn(const std::string& date) const {
    return ageOn(date) >= 18;
}

void Person::updateContacts(const std::string& phone, const std::string& email) {
    if (phone.empty() && email.empty()) {
        throw InvalidStudentDataException("контакты человека не могут быть пустыми");
    }
    if (!phone.empty()) phone_ = phone;
    if (!email.empty()) email_ = email;
}

bool Person::matchesName(const std::string& query) const {
    if (query.empty()) return false;
    return lowered(fullName()).find(lowered(query)) != std::string::npos;
}

void Person::changeAddress(const std::string& address) {
    if (address.empty()) throw InvalidStudentDataException("адрес не может быть пустым");
    address_ = address;
}

} // namespace studentis
