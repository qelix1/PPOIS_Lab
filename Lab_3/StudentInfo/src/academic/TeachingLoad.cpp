#include "studentis/academic/TeachingLoad.h"

#include "studentis/academic/Course.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

TeachingLoad::TeachingLoad(Teacher* teacher, Semester* semester, int plannedHours,
                           int conductedHours)
    : teacher_(teacher),
      semester_(semester),
      plannedHours_(plannedHours),
      conductedHours_(conductedHours) {
    if (plannedHours_ < 0) throw InvalidStudentDataException("плановая нагрузка не может быть отрицательной");
    if (conductedHours_ < 0) {
        throw InvalidStudentDataException("проведённые часы не могут быть отрицательными");
    }
}

void TeachingLoad::addCourse(Course& course) {
    for (Course* current : courses_) {
        if (current == &course) return;
    }
    courses_.push_back(&course);
}

void TeachingLoad::registerConductedHours(int hours) {
    if (hours <= 0) {
        throw InvalidStudentDataException("число проведённых часов должно быть положительным");
    }
    conductedHours_ += hours;
}

int TeachingLoad::remainingHours() const {
    return plannedHours_ - conductedHours_;
}

int TeachingLoad::overloadHours() const {
    const int overload = conductedHours_ - plannedHours_;
    return overload > 0 ? overload : 0;
}

double TeachingLoad::completionPercent() const {
    if (plannedHours_ <= 0) return 0.0;
    return 100.0 * conductedHours_ / plannedHours_;
}

bool TeachingLoad::isOverloaded() const {
    return conductedHours_ > plannedHours_;
}

} // namespace studentis
