#include "studentis/services/ProgressReport.h"

#include <string>

#include "studentis/assessment/AttendanceJournal.h"
#include "studentis/assessment/GradeBook.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

ProgressReport::ProgressReport(std::string id, Student* student, Semester* semester)
    : id_(std::move(id)), student_(student), semester_(semester) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор отчёта не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("отчёт должен относиться к студенту");
}

void ProgressReport::buildFrom(const GradeBook& gradeBook, const AttendanceJournal& journal,
                               const std::string& generatedAt) {
    if (!date::isDate(generatedAt)) throw InvalidStudentDataException("некорректная дата отчёта");
    if (gradeBook.getStudent() != student_) {
        throw InvalidStudentDataException("зачётная книжка принадлежит другому студенту");
    }
    generatedAt_ = generatedAt;
    averageScore_ = gradeBook.average();
    debtsCount_ = static_cast<int>(gradeBook.debtDisciplines().size());
    attendancePercent_ = journal.attendanceRate();
    recommendations_ = (averageScore_ >= 85.0 && debtsCount_ == 0 && attendancePercent_ >= 90.0)
                           ? "нет замечаний"
                           : "требуется улучшение успеваемости";
}

bool ProgressReport::isSuccessful() const {
    return averageScore_ >= 60.0 && debtsCount_ == 0;
}

bool ProgressReport::hasDebts() const {
    return debtsCount_ > 0;
}

std::string ProgressReport::summaryText() const {
    std::string result = "Отчёт " + id_ + " от " + generatedAt_ + ": средний балл " +
                         std::to_string(static_cast<int>(averageScore_)) + ", задолженностей " +
                         std::to_string(debtsCount_) + ", посещаемость " +
                         std::to_string(static_cast<int>(attendancePercent_)) + "%";
    if (!recommendations_.empty()) result += ". Рекомендации: " + recommendations_;
    return result;
}

bool ProgressReport::needsImprovement() const {
    return averageScore_ < 75.0 || debtsCount_ > 0 || attendancePercent_ < 75.0;
}

bool ProgressReport::compareWith(const ProgressReport& other) const {
    if (averageScore_ != other.averageScore_) return averageScore_ > other.averageScore_;
    if (attendancePercent_ != other.attendancePercent_) {
        return attendancePercent_ > other.attendancePercent_;
    }
    return debtsCount_ < other.debtsCount_;
}

} // namespace studentis
