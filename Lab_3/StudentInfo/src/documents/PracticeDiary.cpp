#include "studentis/documents/PracticeDiary.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

PracticeDiary::PracticeDiary(std::string id, Student* student, Teacher* supervisor,
                             std::string organization, std::string startDate, std::string endDate,
                             int hours, std::vector<std::string> tasks, int finalScore)
    : id_(std::move(id)),
      student_(student),
      supervisor_(supervisor),
      organization_(std::move(organization)),
      startDate_(std::move(startDate)),
      endDate_(std::move(endDate)),
      tasks_(std::move(tasks)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор дневника не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("дневник должен относиться к студенту");
    if (organization_.empty()) throw InvalidStudentDataException("организация практики не указана");
    if (!validateDates()) throw InvalidStudentDataException("некорректные даты практики");
    setHours(hours);
    rate(finalScore);
}

void PracticeDiary::addTask(const std::string& task) {
    if (task.empty()) throw InvalidStudentDataException("задача практики не может быть пустой");
    tasks_.push_back(task);
}

int PracticeDiary::totalHours() const {
    return hours_;
}

bool PracticeDiary::isComplete() const {
    return !tasks_.empty() && finalScore_ >= 60;
}

void PracticeDiary::rate(int score) {
    if (score < 0 || score > 100) {
        throw InvalidGradeException("итоговая оценка практики должна быть от 0 до 100");
    }
    finalScore_ = score;
}

int PracticeDiary::durationDays() const {
    return static_cast<int>(date::daysBetween(startDate_, endDate_)) + 1;
}

std::string PracticeDiary::summary() const {
    std::string result = "Дневник практики № " + id_ + ": " + organization_ + ", " + startDate_ +
                         " — " + endDate_ + ", " + std::to_string(hours_) + " ч";
    result += ", задач: " + std::to_string(tasks_.size());
    result += finalScore_ > 0 ? ", оценка " + std::to_string(finalScore_) : ", без оценки";
    return result;
}

bool PracticeDiary::validateDates() const {
    if (!date::isDate(startDate_) || !date::isDate(endDate_)) return false;
    return date::daysBetween(startDate_, endDate_) >= 0;
}

void PracticeDiary::setHours(int hours) {
    if (hours < 0) throw InvalidStudentDataException("часы практики не могут быть отрицательными");
    hours_ = hours;
}

} // namespace studentis
