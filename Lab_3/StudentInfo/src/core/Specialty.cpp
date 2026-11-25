#include "studentis/core/Specialty.h"

#include <algorithm>
#include <cctype>

#include "studentis/core/Curriculum.h"
#include "studentis/core/DateUtils.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

std::string lowered(const std::string& value) {
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

} // namespace

Specialty::Specialty(std::string code, std::string name, Faculty* faculty,
                     std::string qualification, int studyYears, double tuitionFee, int budgetSeats)
    : code_(std::move(code)),
      name_(std::move(name)),
      faculty_(faculty),
      qualification_(std::move(qualification)),
      studyYears_(studyYears),
      tuitionFee_(tuitionFee),
      budgetSeats_(budgetSeats) {
    if (code_.empty()) throw InvalidStudentDataException("код специальности не может быть пустым");
    if (name_.empty()) throw InvalidStudentDataException("название специальности не может быть пустым");
    if (studyYears_ < 1 || studyYears_ > 7) {
        throw InvalidStudentDataException("срок обучения должен быть от 1 до 7 лет");
    }
    if (tuitionFee_ < 0.0) throw InvalidStudentDataException("стоимость обучения не может быть отрицательной");
    if (budgetSeats_ < 0) throw InvalidStudentDataException("число бюджетных мест не может быть отрицательным");
}

bool Specialty::isLongProgram() const {
    return studyYears_ >= 6;
}

double Specialty::annualFee() const {
    if (studyYears_ <= 0) return 0.0;
    return tuitionFee_ / studyYears_;
}

bool Specialty::hasBudgetSeats() const {
    return budgetSeats_ > 0;
}

int Specialty::durationMonths() const {
    return studyYears_ * 10;
}

bool Specialty::matchesCode(const std::string& code) const {
    return lowered(code_) == lowered(code);
}

Curriculum Specialty::createCurriculum(std::string id, int admissionYear) {
    return Curriculum(std::move(id), this, admissionYear, date::today());
}

void Specialty::rename(const std::string& name) {
    if (name.empty()) throw InvalidStudentDataException("название специальности не может быть пустым");
    name_ = name;
}

} // namespace studentis
