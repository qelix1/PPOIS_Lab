#include "studentis/assessment/Grade.h"

#include "studentis/academic/Discipline.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownControlForm(const std::string& form) {
    return form == "exam" || form == "credit" || form == "differentiated_credit";
}

} // namespace

Grade::Grade(std::string id, Student* student, Discipline* discipline, Teacher* teacher, int value,
             std::string date, std::string controlForm, Semester* semester, std::string comment)
    : id_(std::move(id)),
      student_(student),
      discipline_(discipline),
      teacher_(teacher),
      date_(std::move(date)),
      controlForm_(std::move(controlForm)),
      semester_(semester),
      comment_(std::move(comment)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор оценки не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("оценка должна принадлежать студенту");
    if (discipline_ == nullptr) throw InvalidStudentDataException("оценка должна относиться к дисциплине");
    if (!date::isDate(date_)) throw InvalidStudentDataException("некорректная дата оценки: " + date_);
    if (!isKnownControlForm(controlForm_)) {
        throw InvalidStudentDataException("неизвестная форма контроля: " + controlForm_);
    }
    updateValue(value);
}

bool Grade::isPass() const {
    return value_ >= 60;
}

bool Grade::isExcellent() const {
    return value_ >= 85;
}

bool Grade::isFail() const {
    return value_ < 60;
}

void Grade::updateValue(int value) {
    if (value < 0 || value > 100) throw InvalidGradeException("балл должен быть от 0 до 100");
    value_ = value;
}

bool Grade::matchesSemester(const Semester& semester) const {
    return semester_ == &semester;
}

double Grade::weight() const {
    if (controlForm_ == "exam") return 1.0;
    if (controlForm_ == "differentiated_credit") return 0.8;
    return 0.5;
}

std::string Grade::summary() const {
    const std::string discipline = discipline_ != nullptr ? discipline_->getName() : "—";
    std::string text = "оценка " + id_ + " по " + discipline + ": " + std::to_string(value_) + " (" +
                       controlForm_ + ")";
    if (!comment_.empty()) text += ", " + comment_;
    return text;
}

} // namespace studentis
