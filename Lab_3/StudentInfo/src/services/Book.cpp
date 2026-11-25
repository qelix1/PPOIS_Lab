#include "studentis/services/Book.h"

#include <cctype>
#include <string>

#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

/// Нижний регистр для латиницы и кириллицы (UTF-8).
std::string lowered(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    for (std::size_t i = 0; i < value.size();) {
        const unsigned char first = static_cast<unsigned char>(value[i]);
        if (first == 0xD0 && i + 1 < value.size()) {
            const unsigned char second = static_cast<unsigned char>(value[i + 1]);
            if (second >= 0x90 && second <= 0x9F) {
                result += static_cast<char>(0xD0);
                result += static_cast<char>(second + 0x20);
            } else if (second >= 0xA0 && second <= 0xAF) {
                result += static_cast<char>(0xD1);
                result += static_cast<char>(second - 0x20);
            } else if (second == 0x81) {
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

} // namespace

Book::Book(std::string id, std::string title, std::string author, std::string isbn, int year,
           int copiesTotal, int copiesAvailable, std::string category)
    : id_(std::move(id)),
      title_(std::move(title)),
      author_(std::move(author)),
      isbn_(std::move(isbn)),
      year_(year),
      copiesTotal_(copiesTotal),
      copiesAvailable_(copiesAvailable),
      category_(std::move(category)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор книги не может быть пустым");
    if (title_.empty()) throw InvalidStudentDataException("название книги не может быть пустым");
    if (author_.empty()) throw InvalidStudentDataException("автор книги не может быть пустым");
    if (year_ < 1) throw InvalidStudentDataException("год издания книги должен быть положительным");
    if (copiesTotal_ < 0) throw InvalidStudentDataException("число экземпляров не может быть отрицательным");
    if (copiesAvailable_ < 0 || copiesAvailable_ > copiesTotal_) {
        throw InvalidStudentDataException("число свободных экземпляров вне диапазона 0..copiesTotal");
    }
}

bool Book::isAvailable() const {
    return copiesAvailable_ > 0;
}

bool Book::borrow() {
    if (!isAvailable()) return false;
    --copiesAvailable_;
    return true;
}

bool Book::returnCopy() {
    if (copiesAvailable_ >= copiesTotal_) return false;
    ++copiesAvailable_;
    return true;
}

bool Book::matchesAuthor(const std::string& query) const {
    if (query.empty()) return false;
    return lowered(author_).find(lowered(query)) != std::string::npos;
}

bool Book::matchesTitle(const std::string& query) const {
    if (query.empty()) return false;
    return lowered(title_).find(lowered(query)) != std::string::npos;
}

bool Book::isNew(int referenceYear) const {
    return year_ <= referenceYear && referenceYear - year_ <= 5;
}

void Book::reserve() {
    if (!isAvailable()) {
        throw InvalidStudentDataException("нет свободных экземпляров книги " + title_);
    }
    --copiesAvailable_;
}

} // namespace studentis
