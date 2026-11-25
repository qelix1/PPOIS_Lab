#include "studentis/core/Department.h"

#include "studentis/academic/Discipline.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Department::Department(std::string id, std::string name, Faculty* faculty, Teacher* head,
                       std::string phone)
    : id_(std::move(id)),
      name_(std::move(name)),
      faculty_(faculty),
      head_(head),
      phone_(std::move(phone)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор кафедры не может быть пустым");
    if (name_.empty()) throw InvalidStudentDataException("название кафедры не может быть пустым");
}

void Department::addTeacher(Teacher& teacher) {
    for (Teacher* current : teachers_) {
        if (current == &teacher) return;
    }
    teachers_.push_back(&teacher);
    teacher.setDepartment(this);
}

bool Department::removeTeacher(const std::string& teacherId) {
    for (auto it = teachers_.begin(); it != teachers_.end(); ++it) {
        if ((*it)->getTeacherId() == teacherId) {
            teachers_.erase(it);
            return true;
        }
    }
    return false;
}

void Department::assignDiscipline(Discipline& discipline) {
    for (Discipline* current : disciplines_) {
        if (current == &discipline) return;
    }
    disciplines_.push_back(&discipline);
}

std::vector<Teacher*> Department::teachersWithDegree() const {
    std::vector<Teacher*> result;
    for (Teacher* current : teachers_) {
        if (!current->getAcademicDegree().empty()) result.push_back(current);
    }
    return result;
}

void Department::appointHead(Teacher& head) {
    head_ = &head;
}

Discipline* Department::findDiscipline(const std::string& disciplineId) const {
    for (Discipline* discipline : disciplines_) {
        if (discipline->getId() == disciplineId) return discipline;
    }
    return nullptr;
}

int Department::totalPlannedWorkload() const {
    int hours = 0;
    for (const Discipline* discipline : disciplines_) {
        hours += discipline->getCredits() * 36;
    }
    return hours;
}

} // namespace studentis
