#include "studentis/core/Student.h"

#include "studentis/academic/Discipline.h"
#include "studentis/assessment/Exam.h"
#include "studentis/assessment/Grade.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/documents/AcademicLeave.h"
#include "studentis/exceptions/GroupFullException.h"
#include "studentis/exceptions/GroupNotFoundException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "active" || status == "academic_leave" || status == "expelled" ||
           status == "graduated";
}

} // namespace

Student::Student(std::string id, std::string lastName, std::string firstName, std::string middleName,
                 std::string birthDate, std::string gender, std::string phone, std::string email,
                 std::string address, std::string studentId, std::string enrolledAt,
                 std::string status, std::string educationForm, std::string funding,
                 int courseNumber, std::string notes)
    : Person(std::move(id), std::move(lastName), std::move(firstName), std::move(middleName),
             std::move(birthDate), std::move(gender), std::move(phone), std::move(email),
             std::move(address)),
      studentId_(std::move(studentId)),
      enrolledAt_(std::move(enrolledAt)),
      educationForm_(std::move(educationForm)),
      funding_(std::move(funding)),
      notes_(std::move(notes)) {
    if (studentId_.empty()) throw InvalidStudentDataException("номер студента не может быть пустым");
    if (!enrolledAt_.empty() && !date::isDate(enrolledAt_)) {
        throw InvalidStudentDataException("некорректная дата зачисления: " + enrolledAt_);
    }
    setStatus(status);
    setCourseNumber(courseNumber);
}

void Student::assignGroup(StudentGroup& group) {
    group_ = &group;
    courseNumber_ = group.getCourseNumber();
}

void Student::transferTo(StudentGroup* target) {
    if (target == nullptr) throw GroupNotFoundException("целевая группа для перевода не найдена");
    if (target == group_) return;
    StudentGroup* previous = group_;
    target->addStudent(*this);
    if (previous != nullptr) previous->removeStudent(studentId_);
}

void Student::expel() {
    if (group_ != nullptr) {
        group_->removeStudent(studentId_);
        group_ = nullptr;
    }
    status_ = "expelled";
}

void Student::restore() {
    if (status_ != "expelled") {
        throw InvalidStudentDataException("восстановить можно только отчисленного студента");
    }
    status_ = "active";
}

void Student::advanceCourse() {
    if (courseNumber_ >= 6) throw InvalidStudentDataException("курс не может быть выше шестого");
    ++courseNumber_;
}

bool Student::isActive() const {
    return status_ == "active";
}

bool Student::isBudget() const {
    return funding_ == "budget";
}

void Student::recordGrade(Grade& grade) {
    for (Grade* current : grades_) {
        if (current == &grade) return;
    }
    grades_.push_back(&grade);
}

std::vector<Grade*> Student::gradesForDiscipline(const Discipline& discipline) const {
    std::vector<Grade*> result;
    for (Grade* grade : grades_) {
        if (grade->getDiscipline() == &discipline) result.push_back(grade);
    }
    return result;
}

double Student::averageGrade() const {
    if (grades_.empty()) return 0.0;
    int total = 0;
    for (const Grade* grade : grades_) total += grade->getValue();
    return static_cast<double>(total) / static_cast<double>(grades_.size());
}

bool Student::hasAcademicDebt() const {
    for (const Grade* grade : grades_) {
        if (grade->isFail()) return true;
    }
    return false;
}

bool Student::canTakeExam(const Exam& exam) const {
    if (!isActive() || group_ == nullptr || exam.getGroup() != group_ ||
        exam.getDiscipline() == nullptr) {
        return false;
    }
    return exam.getDiscipline()->canStudy(*this);
}

AcademicLeave Student::requestAcademicLeave(const std::string& startDate,
                                            const std::string& endDate,
                                            const std::string& reason) {
    if (!isActive()) {
        throw InvalidStudentDataException("академический отпуск доступен только активному студенту");
    }
    AcademicLeave leave("LEAVE-" + studentId_, this, startDate, endDate, reason,
                        "LEAVE-" + studentId_, "pending", nullptr);
    status_ = "academic_leave";
    return leave;
}

void Student::setStatus(const std::string& status) {
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус студента: " + status);
    status_ = status;
}

void Student::setCourseNumber(int courseNumber) {
    if (courseNumber < 1 || courseNumber > 6) {
        throw InvalidStudentDataException("курс студента должен быть от 1 до 6");
    }
    courseNumber_ = courseNumber;
}

} // namespace studentis
