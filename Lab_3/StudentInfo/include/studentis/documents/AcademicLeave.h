#ifndef STUDENTIS_DOCUMENTS_ACADEMICLEAVE_H
#define STUDENTIS_DOCUMENTS_ACADEMICLEAVE_H

#include <string>

namespace studentis {

class Student;
class Teacher;

/**
 * @brief Академический отпуск: срок, причина, приказ и согласование.
 *
 * Статусы: pending, approved, closed.
 */
class AcademicLeave {
public:
    AcademicLeave() = default;
    AcademicLeave(std::string id, Student* student, std::string startDate, std::string endDate,
                  std::string reason, std::string orderNumber, std::string status,
                  Teacher* approvedBy);

    /// Согласовать отпуск преподавателем.
    void approve(Teacher& teacher);
    /// Продлить согласованный отпуск; новая дата должна быть позже текущей.
    void extend(const std::string& endDate);
    /// Закрыть отпуск.
    void close();
    /// Отпуск действует на указанную дату.
    bool isActive(const std::string& date) const;
    /// Длительность отпуска в днях включительно.
    int durationDays() const;
    /// Пересечение периодов двух отпусков.
    bool overlaps(const AcademicLeave& other) const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    const std::string& getStartDate() const { return startDate_; }
    const std::string& getEndDate() const { return endDate_; }
    const std::string& getReason() const { return reason_; }
    const std::string& getOrderNumber() const { return orderNumber_; }
    const std::string& getStatus() const { return status_; }
    Teacher* getApprovedBy() const { return approvedBy_; }

    void setReason(const std::string& reason) { reason_ = reason; }

private:
    std::string id_;
    Student* student_ = nullptr;
    std::string startDate_;
    std::string endDate_;
    std::string reason_;
    std::string orderNumber_;
    std::string status_ = "pending";
    Teacher* approvedBy_ = nullptr;
};

} // namespace studentis

#endif
