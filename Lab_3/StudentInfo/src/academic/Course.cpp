#include "studentis/academic/Course.h"

#include "studentis/academic/Classroom.h"
#include "studentis/academic/Discipline.h"
#include "studentis/academic/Lesson.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/ClassroomCapacityException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "planned" || status == "active" || status == "finished" ||
           status == "cancelled";
}

} // namespace

Course::Course(std::string id, Discipline* discipline, Teacher* teacher, Semester* semester,
               StudentGroup* group, int plannedHours, Classroom* classroom, std::string status)
    : id_(std::move(id)),
      discipline_(discipline),
      teacher_(teacher),
      semester_(semester),
      group_(group),
      plannedHours_(plannedHours),
      classroom_(classroom),
      status_(std::move(status)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор курса не может быть пустым");
    if (plannedHours_ < 0) throw InvalidStudentDataException("плановые часы не могут быть отрицательными");
    changeStatus(status_);
}

void Course::assignTeacher(Teacher& teacher) {
    teacher_ = &teacher;
    if (discipline_ != nullptr) teacher.assignDiscipline(*discipline_);
}

void Course::assignClassroom(Classroom& classroom) {
    if (group_ != nullptr && !classroom.fits(*group_)) {
        throw ClassroomCapacityException("группа не помещается в аудиторию " + classroom.getRoomNumber());
    }
    classroom_ = &classroom;
}

void Course::addLesson(Lesson& lesson) {
    if (lesson.getCourse() != nullptr && lesson.getCourse() != this) {
        throw InvalidStudentDataException("занятие уже привязано к другому курсу");
    }
    if (lesson.getClassroom() != nullptr) {
        if (group_ != nullptr && !lesson.getClassroom()->fits(*group_)) {
            throw ClassroomCapacityException("группа не помещается в аудиторию " +
                                             lesson.getClassroom()->getRoomNumber());
        }
        if (!lesson.getClassroom()->supports(lesson)) {
            throw ClassroomCapacityException("аудитория не оборудована для занятия типа " +
                                             lesson.getType());
        }
    }
    lesson.setCourse(this);
    for (Lesson* current : lessons_) {
        if (current == &lesson) return;
    }
    lessons_.push_back(&lesson);
}

double Course::completionPercent() const {
    if (plannedHours_ <= 0) return 0.0;
    int conductedMinutes = 0;
    for (const Lesson* lesson : lessons_) {
        if (lesson->isConducted()) conductedMinutes += lesson->durationMinutes();
    }
    const double percent = 100.0 * (conductedMinutes / 60.0) / plannedHours_;
    return percent > 100.0 ? 100.0 : percent;
}

bool Course::isFinished() const {
    return status_ == "finished" || completionPercent() >= 100.0;
}

int Course::studentsCount() const {
    return group_ != nullptr ? group_->activeCount() : 0;
}

void Course::changeStatus(const std::string& status) {
    if (!isKnownStatus(status)) {
        throw InvalidStudentDataException("неизвестный статус курса: " + status);
    }
    status_ = status;
}

bool Course::matchesGroup(const StudentGroup& group) const {
    return group_ == &group;
}

} // namespace studentis
