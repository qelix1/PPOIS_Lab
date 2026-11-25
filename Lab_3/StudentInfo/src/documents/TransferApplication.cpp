#include "studentis/documents/TransferApplication.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/GroupNotFoundException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/StudentNotFoundException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "pending" || status == "approved" || status == "rejected" ||
           status == "applied";
}

} // namespace

TransferApplication::TransferApplication(std::string id, Student* student, StudentGroup* fromGroup,
                                         StudentGroup* toGroup, std::string reason,
                                         std::string createdAt, std::string status,
                                         Teacher* decidedBy, std::string decisionDate)
    : id_(std::move(id)),
      student_(student),
      fromGroup_(fromGroup),
      toGroup_(toGroup),
      reason_(std::move(reason)),
      createdAt_(std::move(createdAt)),
      decidedBy_(decidedBy),
      decisionDate_(std::move(decisionDate)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор заявления не может быть пустым");
    if (!date::isDate(createdAt_)) {
        throw InvalidStudentDataException("некорректная дата создания заявления");
    }
    if (!decisionDate_.empty() && !date::isDate(decisionDate_)) {
        throw InvalidStudentDataException("некорректная дата решения по заявлению");
    }
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус заявления: " + status);
    status_ = status;
}

void TransferApplication::approve(Teacher& teacher, const std::string& decisionDate) {
    if (status_ != "pending") {
        throw InvalidStudentDataException("утвердить можно только заявление без решения");
    }
    if (!isValid()) {
        throw InvalidStudentDataException("заявление о переводе некорректно и не может быть утверждено");
    }
    if (!date::isDate(decisionDate)) {
        throw InvalidStudentDataException("некорректная дата решения по заявлению");
    }
    decidedBy_ = &teacher;
    decisionDate_ = decisionDate;
    status_ = "approved";
}

void TransferApplication::reject(Teacher& teacher, const std::string& decisionDate) {
    if (status_ != "pending") {
        throw InvalidStudentDataException("отклонить можно только заявление без решения");
    }
    if (!date::isDate(decisionDate)) {
        throw InvalidStudentDataException("некорректная дата решения по заявлению");
    }
    decidedBy_ = &teacher;
    decisionDate_ = decisionDate;
    status_ = "rejected";
}

bool TransferApplication::isPending() const {
    return status_ == "pending";
}

bool TransferApplication::isValid() const {
    if (student_ == nullptr || fromGroup_ == nullptr || toGroup_ == nullptr) return false;
    if (fromGroup_ == toGroup_) return false;
    if (student_->getGroup() != fromGroup_) return false;
    return fromGroup_->findStudent(student_->getStudentId()) != nullptr;
}

void TransferApplication::apply() {
    if (status_ != "approved") {
        throw InvalidStudentDataException("исполнить можно только утверждённое заявление");
    }
    if (student_ == nullptr) throw StudentNotFoundException("студент заявления не найден");
    if (toGroup_ == nullptr) throw GroupNotFoundException("целевая группа заявления не найдена");
    if (!isValid()) {
        throw InvalidStudentDataException("заявление о переводе уже неактуально");
    }
    student_->transferTo(toGroup_);
    status_ = "applied";
}

std::string TransferApplication::summary() const {
    std::string result = "Заявление № " + id_ + " от " + createdAt_ + ": ";
    result += student_ != nullptr ? student_->shortName() : "студент не указан";
    result += " ";
    result += fromGroup_ != nullptr ? fromGroup_->getCode() : "?";
    result += " → ";
    result += toGroup_ != nullptr ? toGroup_->getCode() : "?";
    result += " (" + status_ + ")";
    return result;
}

} // namespace studentis
