#include "studentis/assessment/Retake.h"

#include "studentis/assessment/ExamResult.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Retake::Retake(std::string id, ExamResult* result, Student* student, std::string scheduledDate,
               Teacher* teacher, std::string reason)
    : id_(std::move(id)),
      result_(result),
      student_(student),
      scheduledDate_(std::move(scheduledDate)),
      teacher_(teacher),
      reason_(std::move(reason)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор пересдачи не может быть пустым");
    if (result_ == nullptr) throw InvalidStudentDataException("пересдача должна относиться к результату экзамена");
    if (student_ == nullptr) throw InvalidStudentDataException("пересдача должна принадлежать студенту");
    if (teacher_ == nullptr) throw InvalidStudentDataException("у пересдачи должен быть преподаватель");
    if (!date::isDate(scheduledDate_)) {
        throw InvalidStudentDataException("некорректная дата пересдачи: " + scheduledDate_);
    }
    if (result_->getStudent() != student_) {
        throw InvalidStudentDataException("результат экзамена принадлежит другому студенту");
    }
}

void Retake::schedule() {
    if (status_ != "pending") {
        throw InvalidStudentDataException("пересдача уже запланирована или завершена");
    }
    if (!canRetake()) throw InvalidGradeException("экзамен сдан, пересдача не требуется");
    status_ = "scheduled";
}

void Retake::complete(int newScore) {
    if (status_ != "scheduled") throw InvalidStudentDataException("пересдача не запланирована");
    if (!canRetake()) throw InvalidGradeException("экзамен сдан, пересдача не требуется");
    if (newScore < 0 || newScore > 100) throw InvalidGradeException("балл должен быть от 0 до 100");
    newScore_ = newScore;
    status_ = "completed";
}

bool Retake::isPending() const {
    return status_ == "pending";
}

bool Retake::canRetake() const {
    return result_ != nullptr && result_->needsRetake() && status_ != "completed" &&
           status_ != "cancelled";
}

bool Retake::isSuccessful() const {
    return status_ == "completed" && newScore_ >= 60;
}

void Retake::cancel() {
    if (status_ == "completed") throw InvalidStudentDataException("завершённую пересдачу нельзя отменить");
    status_ = "cancelled";
}

} // namespace studentis
