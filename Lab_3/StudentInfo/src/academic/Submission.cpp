#include "studentis/academic/Submission.h"

#include "studentis/academic/Assignment.h"
#include "studentis/core/DateUtils.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Submission::Submission(std::string id, Assignment* assignment, Student* student,
                       std::string submittedAt, std::string content)
    : id_(std::move(id)),
      assignment_(assignment),
      student_(student),
      submittedAt_(std::move(submittedAt)),
      content_(std::move(content)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор работы не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("работа должна принадлежать студенту");
    if (!date::isDate(submittedAt_.substr(0, 10))) {
        throw InvalidStudentDataException("некорректная дата сдачи: " + submittedAt_);
    }
}

void Submission::submit() {
    status_ = "submitted";
}

void Submission::grade(int score, Teacher& checker) {
    if (score < 0 || score > 100) {
        throw InvalidGradeException("балл должен быть от 0 до 100");
    }
    if (assignment_ != nullptr && score > assignment_->getMaxScore()) {
        throw InvalidGradeException("балл " + std::to_string(score) +
                                    " превышает максимальный балл задания " +
                                    std::to_string(assignment_->getMaxScore()));
    }
    score_ = score;
    status_ = "graded";
    checkedBy_ = &checker;
}

bool Submission::isLate() const {
    return assignment_ != nullptr && submittedAt_.substr(0, 10) > assignment_->getDeadline();
}

bool Submission::needsRevision() const {
    return status_ == "revision";
}

void Submission::returnForRevision(const std::string& feedback, Teacher& checker) {
    feedback_ = feedback;
    checkedBy_ = &checker;
    status_ = "revision";
}

bool Submission::isGraded() const {
    return status_ == "graded";
}

std::string Submission::summary() const {
    return "работа " + id_ + ": " + status_ + (score_ >= 0 ? ", балл " + std::to_string(score_) : "");
}

} // namespace studentis
