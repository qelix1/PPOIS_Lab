#include "studentis/core/Teacher.h"

#include <cmath>

#include "studentis/academic/Discipline.h"
#include "studentis/assessment/Grade.h"
#include "studentis/core/DateUtils.h"
#include "studentis/documents/Thesis.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Teacher::Teacher(std::string id, std::string lastName, std::string firstName, std::string middleName,
                 std::string birthDate, std::string gender, std::string phone, std::string email,
                 std::string address, std::string teacherId, std::string position,
                 std::string academicDegree, std::string academicTitle, std::string hiredAt,
                 int baseWorkloadHours)
    : Person(std::move(id), std::move(lastName), std::move(firstName), std::move(middleName),
             std::move(birthDate), std::move(gender), std::move(phone), std::move(email),
             std::move(address)),
      teacherId_(std::move(teacherId)),
      position_(std::move(position)),
      academicDegree_(std::move(academicDegree)),
      academicTitle_(std::move(academicTitle)),
      hiredAt_(std::move(hiredAt)),
      baseWorkloadHours_(baseWorkloadHours) {
    if (teacherId_.empty()) throw InvalidStudentDataException("номер преподавателя не может быть пустым");
    if (!hiredAt_.empty() && !date::isDate(hiredAt_)) {
        throw InvalidStudentDataException("некорректная дата приёма на работу: " + hiredAt_);
    }
    if (baseWorkloadHours_ < 0) {
        throw InvalidStudentDataException("базовая нагрузка не может быть отрицательной");
    }
}

void Teacher::assignDiscipline(Discipline& discipline) {
    for (Discipline* current : disciplines_) {
        if (current == &discipline) return;
    }
    disciplines_.push_back(&discipline);
}

void Teacher::removeDiscipline(const Discipline& discipline) {
    for (auto it = disciplines_.begin(); it != disciplines_.end(); ++it) {
        if (*it == &discipline) {
            disciplines_.erase(it);
            return;
        }
    }
}

bool Teacher::canTeach(const Discipline& discipline) const {
    for (const Discipline* current : disciplines_) {
        if (current == &discipline) return true;
    }
    return false;
}

bool Teacher::isProfessor() const {
    return position_ == "professor" || academicTitle_ == "professor";
}

int Teacher::yearsOfService(const std::string& date) const {
    if (!date::isDate(hiredAt_)) {
        throw InvalidStudentDataException("некорректная дата приёма на работу: " + hiredAt_);
    }
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата: " + date);
    int years = std::stoi(date.substr(0, 4)) - std::stoi(hiredAt_.substr(0, 4));
    if (date.substr(5, 5) < hiredAt_.substr(5, 5)) --years;
    return years < 0 ? 0 : years;
}

int Teacher::plannedWorkload() const {
    int credits = 0;
    for (const Discipline* discipline : disciplines_) {
        credits += discipline->getCredits();
    }
    return credits * 36;
}

int Teacher::workloadPercent() const {
    if (baseWorkloadHours_ <= 0) return 0;
    return static_cast<int>(std::lround(plannedWorkload() * 100.0 / baseWorkloadHours_));
}

void Teacher::superviseThesis(Thesis& thesis) {
    thesis.assignReviewer(*this);
}

void Teacher::markGrade(Grade& grade) {
    grade.setTeacher(this);
}

} // namespace studentis
