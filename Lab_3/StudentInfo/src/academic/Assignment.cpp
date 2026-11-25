#include "studentis/academic/Assignment.h"

#include "studentis/academic/Submission.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Assignment::Assignment(std::string id, Course* course, std::string title, std::string description,
                       std::string issuedAt, std::string deadline, int maxScore, std::string type)
    : id_(std::move(id)),
      course_(course),
      title_(std::move(title)),
      description_(std::move(description)),
      issuedAt_(std::move(issuedAt)),
      deadline_(std::move(deadline)),
      maxScore_(maxScore),
      type_(std::move(type)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор задания не может быть пустым");
    if (title_.empty()) throw InvalidStudentDataException("название задания не может быть пустым");
    if (!date::isDate(issuedAt_) || !date::isDate(deadline_)) {
        throw InvalidStudentDataException("некорректные даты задания");
    }
    if (issuedAt_ > deadline_) throw InvalidStudentDataException("срок сдачи раньше даты выдачи");
    if (maxScore_ <= 0) throw InvalidStudentDataException("максимальный балл должен быть положительным");
}

void Assignment::addSubmission(Submission& submission) {
    if (submission.getAssignment() != nullptr && submission.getAssignment() != this) {
        throw InvalidStudentDataException("работа относится к другому заданию");
    }
    if (submission.getScore() > maxScore_) {
        throw InvalidGradeException("балл " + std::to_string(submission.getScore()) +
                                    " превышает максимальный балл задания " +
                                    std::to_string(maxScore_));
    }
    for (const Submission* current : submissions_) {
        if (current == &submission) return;
    }
    if (submission.getStudent() != nullptr && hasSubmission(*submission.getStudent())) return;
    submission.setAssignment(this);
    submissions_.push_back(&submission);
}

Submission* Assignment::submissionFor(const Student& student) const {
    for (Submission* submission : submissions_) {
        if (submission->getStudent() == &student) return submission;
    }
    return nullptr;
}

bool Assignment::isOverdue(const std::string& date) const {
    if (!date::isDate(date)) return false;
    return date > deadline_;
}

int Assignment::gradedCount() const {
    int count = 0;
    for (const Submission* submission : submissions_) {
        if (submission->isGraded()) ++count;
    }
    return count;
}

double Assignment::averageScore() const {
    int sum = 0;
    int count = 0;
    for (const Submission* submission : submissions_) {
        if (!submission->isGraded()) continue;
        sum += submission->getScore();
        ++count;
    }
    return count == 0 ? 0.0 : static_cast<double>(sum) / count;
}

double Assignment::completionPercent() const {
    if (submissions_.empty()) return 0.0;
    return 100.0 * gradedCount() / static_cast<double>(submissions_.size());
}

void Assignment::extendDeadline(const std::string& newDeadline) {
    if (!date::isDate(newDeadline)) {
        throw InvalidStudentDataException("некорректный срок сдачи: " + newDeadline);
    }
    if (newDeadline < issuedAt_) {
        throw InvalidStudentDataException("срок сдачи не может быть раньше даты выдачи");
    }
    deadline_ = newDeadline;
}

bool Assignment::hasSubmission(const Student& student) const {
    return submissionFor(student) != nullptr;
}

} // namespace studentis
