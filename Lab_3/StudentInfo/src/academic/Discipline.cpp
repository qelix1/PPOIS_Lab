#include "studentis/academic/Discipline.h"

#include <cctype>

#include "studentis/assessment/Grade.h"
#include "studentis/core/Department.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

std::string toLower(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    for (std::size_t i = 0; i < text.size();) {
        const unsigned char first = static_cast<unsigned char>(text[i]);
        if (first == 0xD0 && i + 1 < text.size()) {
            const unsigned char second = static_cast<unsigned char>(text[i + 1]);
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
                result += text[i];
                result += text[i + 1];
            }
            i += 2;
            continue;
        }
        result += static_cast<char>(std::tolower(first));
        ++i;
    }
    return result;
}

bool hasPassingGrade(const Student& student, const Discipline& prerequisite) {
    for (Grade* grade : student.getGrades()) {
        if (grade != nullptr && grade->getDiscipline() == &prerequisite && grade->isPass()) {
            return true;
        }
    }
    return false;
}

} // namespace

Discipline::Discipline(std::string id, std::string name, std::string code, Department* department,
                       int credits, std::string description)
    : id_(std::move(id)),
      name_(std::move(name)),
      code_(std::move(code)),
      department_(department),
      credits_(credits),
      description_(std::move(description)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор дисциплины не может быть пустым");
    if (name_.empty()) throw InvalidStudentDataException("название дисциплины не может быть пустым");
    if (credits_ < 0) throw InvalidStudentDataException("зачётные единицы не могут быть отрицательными");
}

void Discipline::addPrerequisite(Discipline& prerequisite) {
    if (&prerequisite == this) {
        throw InvalidStudentDataException("дисциплина не может быть своим пререквизитом");
    }
    if (hasPrerequisite(prerequisite)) return;
    prerequisites_.push_back(&prerequisite);
}

bool Discipline::hasPrerequisite(const Discipline& prerequisite) const {
    for (const Discipline* current : prerequisites_) {
        if (current == &prerequisite) return true;
    }
    return false;
}

bool Discipline::canStudy(const Student& student) const {
    for (const Discipline* prerequisite : prerequisites_) {
        if (!hasPassingGrade(student, *prerequisite)) return false;
    }
    return true;
}

bool Discipline::matchesName(const std::string& query) const {
    if (query.empty()) return false;
    const std::string needle = toLower(query);
    return toLower(name_).find(needle) != std::string::npos ||
           toLower(code_).find(needle) != std::string::npos;
}

void Discipline::assignDepartment(Department& department) {
    department_ = &department;
    department.assignDiscipline(*this);
}

int Discipline::totalPrerequisites() const {
    return static_cast<int>(prerequisites_.size());
}

std::string Discipline::summary() const {
    return code_ + " " + name_ + ", " + std::to_string(credits_) + " з.е.";
}

} // namespace studentis
