#include "studentis/assessment/Exam.h"

#include "studentis/academic/Classroom.h"
#include "studentis/academic/Discipline.h"
#include "studentis/assessment/ExamTicket.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/ExamRegistrationException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "scheduled" || status == "in_progress" || status == "completed" ||
           status == "cancelled";
}

} // namespace

Exam::Exam(std::string id, Discipline* discipline, StudentGroup* group, Teacher* teacher,
           std::string date, Classroom* classroom, int maxScore, std::string status)
    : id_(std::move(id)),
      discipline_(discipline),
      group_(group),
      teacher_(teacher),
      date_(std::move(date)),
      classroom_(classroom),
      maxScore_(maxScore),
      status_(std::move(status)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор экзамена не может быть пустым");
    if (discipline_ == nullptr) throw InvalidStudentDataException("экзамен должен относиться к дисциплине");
    if (group_ == nullptr) throw InvalidStudentDataException("экзамен должен проводиться для группы");
    if (teacher_ == nullptr) throw InvalidStudentDataException("у экзамена должен быть преподаватель");
    if (!date::isDate(date_)) throw InvalidStudentDataException("некорректная дата экзамена: " + date_);
    if (maxScore_ <= 0 || maxScore_ > 100) {
        throw InvalidStudentDataException("максимальный балл экзамена должен быть от 1 до 100");
    }
    if (!isKnownStatus(status_)) throw InvalidStudentDataException("неизвестный статус экзамена: " + status_);
}

void Exam::addTicket(ExamTicket& ticket) {
    if (ticket.getExam() != this) {
        throw InvalidStudentDataException("билет принадлежит другому экзамену");
    }
    for (const ExamTicket* current : tickets_) {
        if (current->getNumber() == ticket.getNumber()) return;
    }
    tickets_.push_back(&ticket);
}

bool Exam::removeTicket(int number) {
    for (auto it = tickets_.begin(); it != tickets_.end(); ++it) {
        if ((*it)->getNumber() == number) {
            tickets_.erase(it);
            return true;
        }
    }
    return false;
}

bool Exam::isScheduledOn(const std::string& date) const {
    return date::isDate(date) && date_ == date;
}

bool Exam::canStart() const {
    return status_ == "scheduled" && !tickets_.empty();
}

void Exam::registerStudent(Student& student) {
    for (const Student* current : registered_) {
        if (current == &student) {
            throw ExamRegistrationException("студент уже зарегистрирован на экзамен");
        }
    }
    if (!student.isActive()) {
        throw ExamRegistrationException("экзамен доступен только активным студентам");
    }
    if (student.getGroup() != group_) {
        throw ExamRegistrationException("студент не входит в группу экзамена");
    }
    if (status_ != "scheduled") {
        throw ExamRegistrationException("регистрация на экзамен закрыта: статус " + status_);
    }
    registered_.push_back(&student);
}

int Exam::registeredCount() const {
    return static_cast<int>(registered_.size());
}

void Exam::complete() {
    if (status_ == "completed") throw InvalidStudentDataException("экзамен уже завершён");
    if (status_ == "cancelled") throw InvalidStudentDataException("отменённый экзамен нельзя завершить");
    status_ = "completed";
}

std::string Exam::summary() const {
    const std::string discipline = discipline_ != nullptr ? discipline_->getName() : "—";
    return "экзамен " + id_ + " по " + discipline + " (" + date_ + "): " + status_ + ", билетов " +
           std::to_string(tickets_.size()) + ", записано " + std::to_string(registered_.size());
}

} // namespace studentis
