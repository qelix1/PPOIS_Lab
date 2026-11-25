#include "studentis/services/StudentDirectory.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

std::string csvField(const std::string& value) {
    if (value.find_first_of(",\"\n\r") == std::string::npos) return value;
    std::string result = "\"";
    for (const char symbol : value) {
        if (symbol == '"') result += '"';
        result += symbol;
    }
    result += '"';
    return result;
}

} // namespace

void StudentDirectory::addStudent(Student& student) {
    for (const Student* current : students_) {
        if (current == &student || current->getStudentId() == student.getStudentId()) return;
    }
    students_.push_back(&student);
}

void StudentDirectory::addGroup(StudentGroup& group) {
    for (const StudentGroup* current : groups_) {
        if (current == &group || current->getCode() == group.getCode()) return;
    }
    groups_.push_back(&group);
}

void StudentDirectory::addTeacher(Teacher& teacher) {
    for (const Teacher* current : teachers_) {
        if (current == &teacher || current->getTeacherId() == teacher.getTeacherId()) return;
    }
    teachers_.push_back(&teacher);
}

Student* StudentDirectory::findById(const std::string& studentId) const {
    for (Student* student : students_) {
        if (student->getStudentId() == studentId || student->getId() == studentId) return student;
    }
    return nullptr;
}

Student* StudentDirectory::findByName(const std::string& query) const {
    for (Student* student : students_) {
        if (student->matchesName(query)) return student;
    }
    return nullptr;
}

std::vector<Student*> StudentDirectory::filterByGroup(const std::string& groupCode) const {
    std::vector<Student*> result;
    for (Student* student : students_) {
        if (student->getGroup() != nullptr && student->getGroup()->getCode() == groupCode) {
            result.push_back(student);
        }
    }
    return result;
}

std::vector<Student*> StudentDirectory::filterByCourse(int courseNumber) const {
    std::vector<Student*> result;
    for (Student* student : students_) {
        if (student->getCourseNumber() == courseNumber) result.push_back(student);
    }
    return result;
}

std::vector<Student*> StudentDirectory::topStudents(int count) const {
    std::vector<Student*> result;
    if (count <= 0) return result;
    for (Student* student : students_) {
        if (student->isActive()) result.push_back(student);
    }
    std::sort(result.begin(), result.end(), [](const Student* left, const Student* right) {
        if (left->averageGrade() != right->averageGrade()) {
            return left->averageGrade() > right->averageGrade();
        }
        return left->fullName() < right->fullName();
    });
    if (static_cast<int>(result.size()) > count) result.resize(static_cast<std::size_t>(count));
    return result;
}

std::vector<Student*> StudentDirectory::studentsWithDebt() const {
    std::vector<Student*> result;
    for (Student* student : students_) {
        if (student->isActive() && student->hasAcademicDebt()) result.push_back(student);
    }
    return result;
}

std::string StudentDirectory::exportCsv() const {
    std::ostringstream out;
    out << "studentId,fullName,group,course,status,funding,average\n";
    for (const Student* student : students_) {
        const std::string group = student->getGroup() == nullptr ? "" : student->getGroup()->getCode();
        out << csvField(student->getStudentId()) << ',' << csvField(student->fullName()) << ','
            << csvField(group) << ',' << student->getCourseNumber() << ','
            << csvField(student->getStatus()) << ',' << csvField(student->getFunding()) << ','
            << std::fixed << std::setprecision(2) << student->averageGrade() << '\n';
    }
    return out.str();
}

} // namespace studentis
