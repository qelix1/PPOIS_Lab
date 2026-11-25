#include "studentis/documents/EnrollmentOrder.h"

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
    return status == "draft" || status == "signed" || status == "executed" ||
           status == "cancelled";
}

} // namespace

EnrollmentOrder::EnrollmentOrder(std::string id, Student* student, StudentGroup* group,
                                 std::string orderDate, std::string orderNumber,
                                 std::string basis, Teacher* signedBy, std::string status)
    : id_(std::move(id)),
      student_(student),
      group_(group),
      orderDate_(std::move(orderDate)),
      orderNumber_(std::move(orderNumber)),
      basis_(std::move(basis)),
      signedBy_(signedBy) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор приказа не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("приказ должен относиться к студенту");
    if (orderNumber_.empty()) throw InvalidStudentDataException("номер приказа не может быть пустым");
    if (!date::isDate(orderDate_)) throw InvalidStudentDataException("некорректная дата приказа");
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус приказа: " + status);
    status_ = status;
}

void EnrollmentOrder::sign(Teacher& teacher) {
    if (status_ != "draft") {
        throw InvalidStudentDataException("подписать можно только черновик приказа");
    }
    signedBy_ = &teacher;
    status_ = "signed";
}

void EnrollmentOrder::execute() {
    if (status_ != "signed") {
        throw InvalidStudentDataException("исполнить можно только подписанный приказ");
    }
    if (signedBy_ == nullptr) {
        throw InvalidStudentDataException("приказ не подписан преподавателем");
    }
    if (student_ == nullptr) throw StudentNotFoundException("студент приказа не найден");
    if (group_ == nullptr) throw GroupNotFoundException("группа приказа не найдена");
    group_->addStudent(*student_);
    status_ = "executed";
}

void EnrollmentOrder::cancel() {
    if (status_ == "executed") {
        throw InvalidStudentDataException("нельзя отменить исполненный приказ");
    }
    status_ = "cancelled";
}

bool EnrollmentOrder::isExecuted() const {
    return status_ == "executed";
}

bool EnrollmentOrder::isBudget() const {
    return student_ != nullptr && student_->isBudget();
}

std::string EnrollmentOrder::summary() const {
    std::string result = "Приказ № " + orderNumber_ + " от " + orderDate_ + ": ";
    result += student_ != nullptr ? student_->shortName() : "студент не указан";
    result += " → ";
    result += group_ != nullptr ? group_->getCode() : "группа не указана";
    result += " (" + status_ + ")";
    return result;
}

} // namespace studentis
