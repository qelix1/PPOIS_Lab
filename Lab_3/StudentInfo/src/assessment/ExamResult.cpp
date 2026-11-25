#include "studentis/assessment/ExamResult.h"

#include "studentis/assessment/Exam.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

ExamResult::ExamResult(Exam* exam, Student* student, int score, std::string answers,
                       std::string checkedAt)
    : exam_(exam),
      student_(student),
      answers_(std::move(answers)),
      checkedAt_(std::move(checkedAt)) {
    if (exam_ == nullptr) throw InvalidStudentDataException("результат должен относиться к экзамену");
    if (student_ == nullptr) throw InvalidStudentDataException("результат должен принадлежать студенту");
    if (score < 0 || score > 100) throw InvalidGradeException("балл должен быть от 0 до 100");
    if (!checkedAt_.empty() &&
        (!date::isDate(checkedAt_.substr(0, 10)) || date::minutesOfDateTime(checkedAt_) < 0)) {
        throw InvalidStudentDataException("некорректные дата и время проверки: " + checkedAt_);
    }
    score_ = score;
    grade_ = toFiveScale();
}

bool ExamResult::isPassed() const {
    return score_ >= 60;
}

bool ExamResult::isExcellent() const {
    return score_ >= 85;
}

int ExamResult::toFiveScale() const {
    if (score_ < 60) return 2;
    if (score_ < 70) return 3;
    if (score_ < 85) return 4;
    return 5;
}

bool ExamResult::needsRetake() const {
    return !isPassed();
}

void ExamResult::allowRetake() {
    if (!needsRetake()) throw InvalidGradeException("экзамен сдан, пересдача не требуется");
    retakeAllowed_ = true;
}

std::string ExamResult::summary() const {
    return "результат экзамена: балл " + std::to_string(score_) + ", оценка " +
           std::to_string(grade_);
}

} // namespace studentis
