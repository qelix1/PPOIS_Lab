#include "studentis/core/University.h"

#include "studentis/core/AcademicYear.h"
#include "studentis/core/Faculty.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/StudentNotFoundException.h"

namespace studentis {

University::University(std::string name, Teacher* rector, int foundedYear)
    : name_(std::move(name)), rector_(rector), foundedYear_(foundedYear) {
    if (name_.empty()) throw InvalidStudentDataException("название университета не может быть пустым");
    if (foundedYear_ < 1000) throw InvalidStudentDataException("некорректный год основания университета");
}

void University::addFaculty(Faculty& faculty) {
    for (Faculty* current : faculties_) {
        if (current == &faculty) return;
    }
    faculties_.push_back(&faculty);
}

Faculty* University::findFaculty(const std::string& idOrName) const {
    for (Faculty* current : faculties_) {
        if (current->getId() == idOrName || current->getName() == idOrName) return current;
    }
    return nullptr;
}

void University::enrollStudent(Student& student) {
    for (Student* current : students_) {
        if (current == &student) return;
    }
    students_.push_back(&student);
}

void University::expelStudent(const std::string& studentId) {
    for (Student* current : students_) {
        if (current->getStudentId() == studentId) {
            current->expel();
            return;
        }
    }
    throw StudentNotFoundException("студент " + studentId + " не зачислен в университет");
}

int University::totalStudents() const {
    int count = 0;
    for (const Student* current : students_) {
        if (current->isActive()) ++count;
    }
    return count;
}

StudentGroup* University::findGroup(const std::string& code) const {
    for (const Student* current : students_) {
        StudentGroup* group = current->getGroup();
        if (group != nullptr && group->getCode() == code) return group;
    }
    return nullptr;
}

void University::setCurrentYear(AcademicYear& year) {
    if (currentYear_ != nullptr) currentYear_->setCurrent(false);
    currentYear_ = &year;
    year.setCurrent(true);
}

std::string University::summary() const {
    std::string result = name_ + " | факультетов: " + std::to_string(faculties_.size()) +
                         " | студентов: " + std::to_string(totalStudents()) + " | учебный год: ";
    result += currentYear_ != nullptr ? currentYear_->getName() : "не задан";
    return result;
}

} // namespace studentis
