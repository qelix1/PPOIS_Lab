#include "studentis/core/Faculty.h"

#include "studentis/core/Department.h"
#include "studentis/core/Specialty.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Faculty::Faculty(std::string id, std::string name, std::string shortName, Teacher* dean,
                 std::string phone, std::string building)
    : id_(std::move(id)),
      name_(std::move(name)),
      shortName_(std::move(shortName)),
      dean_(dean),
      phone_(std::move(phone)),
      building_(std::move(building)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор факультета не может быть пустым");
    if (name_.empty()) throw InvalidStudentDataException("название факультета не может быть пустым");
}

void Faculty::addDepartment(Department& department) {
    for (Department* current : departments_) {
        if (current == &department) return;
    }
    departments_.push_back(&department);
    department.setFaculty(this);
}

Department* Faculty::findDepartment(const std::string& idOrName) const {
    for (Department* current : departments_) {
        if (current->getId() == idOrName || current->getName() == idOrName) return current;
    }
    return nullptr;
}

std::vector<Specialty*> Faculty::allSpecialties(const std::vector<Specialty*>& specialties) const {
    std::vector<Specialty*> result;
    for (Specialty* current : specialties) {
        if (current != nullptr && current->getFaculty() == this) result.push_back(current);
    }
    return result;
}

void Faculty::appointDean(Teacher& dean) {
    dean_ = &dean;
}

int Faculty::studentCount(const std::vector<Student*>& students) const {
    int count = 0;
    for (const Student* current : students) {
        if (current == nullptr || !current->isActive()) continue;
        StudentGroup* group = current->getGroup();
        if (group != nullptr && group->getSpecialty() != nullptr &&
            group->getSpecialty()->getFaculty() == this) {
            ++count;
        }
    }
    return count;
}

int Faculty::teacherCount() const {
    int count = 0;
    for (const Department* current : departments_) {
        count += static_cast<int>(current->getTeachers().size());
    }
    return count;
}

void Faculty::rename(const std::string& name, const std::string& shortName) {
    if (name.empty() && shortName.empty()) {
        throw InvalidStudentDataException("названия факультета не могут быть пустыми");
    }
    if (!name.empty()) name_ = name;
    if (!shortName.empty()) shortName_ = shortName;
}

} // namespace studentis
