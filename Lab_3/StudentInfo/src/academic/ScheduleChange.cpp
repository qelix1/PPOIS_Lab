#include "studentis/academic/ScheduleChange.h"

#include "studentis/academic/Classroom.h"
#include "studentis/academic/Course.h"
#include "studentis/academic/Lesson.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

ScheduleChange::ScheduleChange(std::string id, Lesson* lesson, std::string oldDate,
                               std::string newDate, Classroom* oldClassroom,
                               Classroom* newClassroom, std::string reason, Teacher* approvedBy,
                               std::string createdAt)
    : id_(std::move(id)),
      lesson_(lesson),
      oldDate_(std::move(oldDate)),
      newDate_(std::move(newDate)),
      oldClassroom_(oldClassroom),
      newClassroom_(newClassroom),
      reason_(std::move(reason)),
      approvedBy_(approvedBy),
      createdAt_(std::move(createdAt)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор изменения не может быть пустым");
    if (!date::isDate(oldDate_) || !date::isDate(newDate_)) {
        throw InvalidStudentDataException("некорректные даты изменения расписания");
    }
    if (!createdAt_.empty() && !date::isDate(createdAt_.substr(0, 10))) {
        throw InvalidStudentDataException("некорректная дата создания изменения");
    }
}

void ScheduleChange::approve(Teacher& teacher) {
    approvedBy_ = &teacher;
}

void ScheduleChange::apply() {
    if (lesson_ == nullptr) throw InvalidStudentDataException("изменение не связано с занятием");
    if (approvedBy_ == nullptr) throw InvalidStudentDataException("изменение не согласовано");
    lesson_->moveTo(newDate_, lesson_->getStartTime(), lesson_->getEndTime());
    if (newClassroom_ != nullptr) lesson_->assignClassroom(*newClassroom_);
}

void ScheduleChange::cancel() {
    if (lesson_ != nullptr && isApplied()) {
        lesson_->moveTo(oldDate_, lesson_->getStartTime(), lesson_->getEndTime());
        if (oldClassroom_ != nullptr) lesson_->assignClassroom(*oldClassroom_);
    }
    approvedBy_ = nullptr;
}

bool ScheduleChange::isApplied() const {
    if (lesson_ == nullptr) return false;
    if (lesson_->getDate() != newDate_) return false;
    return newClassroom_ == nullptr || lesson_->getClassroom() == newClassroom_;
}

bool ScheduleChange::affectsGroup(const StudentGroup& group) const {
    return lesson_ != nullptr && lesson_->getCourse() != nullptr &&
           lesson_->getCourse()->getGroup() == &group;
}

std::string ScheduleChange::summary() const {
    return "перенос занятия " + (lesson_ != nullptr ? lesson_->getId() : std::string("—")) + " с " +
           oldDate_ + " на " + newDate_ + ": " + reason_;
}

} // namespace studentis
