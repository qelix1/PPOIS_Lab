#include "studentis/documents/Thesis.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "draft" || status == "submitted" || status == "defended" ||
           status == "failed";
}

} // namespace

Thesis::Thesis(std::string id, Student* student, Teacher* supervisor, std::string topic,
               std::string submittedAt, std::string defenseDate, double plagiarismPercent,
               int score, std::string status)
    : id_(std::move(id)),
      student_(student),
      supervisor_(supervisor),
      topic_(std::move(topic)),
      submittedAt_(std::move(submittedAt)),
      defenseDate_(std::move(defenseDate)),
      plagiarismPercent_(plagiarismPercent) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор работы не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("работа должна относиться к студенту");
    if (topic_.empty()) throw InvalidStudentDataException("тема работы не может быть пустой");
    if (!submittedAt_.empty() && !date::isDate(submittedAt_)) {
        throw InvalidStudentDataException("некорректная дата сдачи работы");
    }
    if (!defenseDate_.empty() && !date::isDate(defenseDate_)) {
        throw InvalidStudentDataException("некорректная дата защиты работы");
    }
    if (!submittedAt_.empty() && !defenseDate_.empty() &&
        date::daysBetween(submittedAt_, defenseDate_) < 0) {
        throw InvalidStudentDataException("дата защиты раньше даты сдачи работы");
    }
    checkPlagiarism(plagiarismPercent);
    if (score < 0 || score > 100) {
        throw InvalidGradeException("оценка работы должна быть от 0 до 100");
    }
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус работы: " + status);
    score_ = score;
    status_ = status;
}

void Thesis::submit(const std::string& submittedAt) {
    if (topic_.empty()) throw InvalidStudentDataException("тема работы не может быть пустой");
    if (!date::isDate(submittedAt)) throw InvalidStudentDataException("некорректная дата сдачи работы");
    if (!submittedAt_.empty()) throw InvalidStudentDataException("работа уже сдана");
    if (!defenseDate_.empty() && date::daysBetween(submittedAt, defenseDate_) < 0) {
        throw InvalidStudentDataException("дата защиты раньше даты сдачи работы");
    }
    submittedAt_ = submittedAt;
    status_ = "submitted";
}

void Thesis::checkPlagiarism(double plagiarismPercent) {
    if (plagiarismPercent < 0.0 || plagiarismPercent > 100.0) {
        throw InvalidStudentDataException("процент плагиата должен быть от 0 до 100");
    }
    plagiarismPercent_ = plagiarismPercent;
}

void Thesis::assignReviewer(Teacher& teacher) {
    supervisor_ = &teacher;
}

bool Thesis::isAllowedToDefend() const {
    if (submittedAt_.empty() || defenseDate_.empty()) return false;
    if (plagiarismPercent_ > 20.0) return false;
    return status_ != "failed";
}

void Thesis::grade(int score) {
    if (!isAllowedToDefend()) {
        throw InvalidStudentDataException("работа не допущена к защите");
    }
    if (score < 0 || score > 100) {
        throw InvalidGradeException("оценка работы должна быть от 0 до 100");
    }
    score_ = score;
    status_ = score >= 60 ? "defended" : "failed";
}

bool Thesis::isPassed() const {
    return status_ == "defended" && score_ >= 60;
}

int Thesis::daysToDefense(const std::string& date) const {
    if (defenseDate_.empty()) return 0;
    return static_cast<int>(date::daysBetween(date, defenseDate_));
}

void Thesis::setDefenseDate(const std::string& defenseDate) {
    if (!date::isDate(defenseDate)) throw InvalidStudentDataException("некорректная дата защиты работы");
    if (!submittedAt_.empty() && date::daysBetween(submittedAt_, defenseDate) < 0) {
        throw InvalidStudentDataException("дата защиты раньше даты сдачи работы");
    }
    defenseDate_ = defenseDate;
}

} // namespace studentis
