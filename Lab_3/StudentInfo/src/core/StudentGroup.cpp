#include "studentis/core/StudentGroup.h"

#include "studentis/core/Student.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/GroupFullException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

StudentGroup::StudentGroup(std::string code, Specialty* specialty, int admissionYear,
                           int courseNumber, Teacher* curator, int maxSize, std::string status)
    : code_(std::move(code)),
      specialty_(specialty),
      admissionYear_(admissionYear),
      courseNumber_(courseNumber),
      curator_(curator),
      maxSize_(maxSize),
      status_(std::move(status)) {
    if (code_.empty()) throw InvalidStudentDataException("код группы не может быть пустым");
    if (admissionYear_ < 1900) throw InvalidStudentDataException("некорректный год набора группы");
    if (courseNumber_ < 1 || courseNumber_ > 6) {
        throw InvalidStudentDataException("курс группы должен быть от 1 до 6");
    }
    if (maxSize_ <= 0) throw InvalidStudentDataException("размер группы должен быть положительным");
}

void StudentGroup::addStudent(Student& student) {
    for (Student* current : students_) {
        if (current == &student) return;
    }
    if (activeCount() >= maxSize_) {
        throw GroupFullException("в группе " + code_ + " нет свободных мест");
    }
    students_.push_back(&student);
    student.assignGroup(*this);
}

bool StudentGroup::removeStudent(const std::string& studentId) {
    for (auto it = students_.begin(); it != students_.end(); ++it) {
        if ((*it)->getStudentId() == studentId) {
            students_.erase(it);
            return true;
        }
    }
    return false;
}

Student* StudentGroup::findStudent(const std::string& studentId) const {
    for (Student* current : students_) {
        if (current->getStudentId() == studentId) return current;
    }
    return nullptr;
}

int StudentGroup::activeCount() const {
    int count = 0;
    for (const Student* current : students_) {
        if (current->isActive()) ++count;
    }
    return count;
}

bool StudentGroup::isFull() const {
    return activeCount() >= maxSize_;
}

void StudentGroup::advanceCourse() {
    if (courseNumber_ >= 6) throw InvalidStudentDataException("курс группы не может быть выше шестого");
    ++courseNumber_;
}

void StudentGroup::assignCurator(Teacher& curator) {
    curator_ = &curator;
}

int StudentGroup::availableSeats() const {
    int free = maxSize_ - activeCount();
    return free < 0 ? 0 : free;
}

double StudentGroup::averageScore() const {
    int count = 0;
    double total = 0.0;
    for (const Student* student : students_) {
        if (!student->isActive()) continue;
        total += student->averageGrade();
        ++count;
    }
    return count == 0 ? 0.0 : total / count;
}

std::vector<Student*> StudentGroup::studentsWithDebts() const {
    std::vector<Student*> result;
    for (Student* student : students_) {
        if (student->isActive() && student->hasAcademicDebt()) result.push_back(student);
    }
    return result;
}

} // namespace studentis
