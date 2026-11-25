#ifndef STUDENTIS_FINANCE_SCHOLARSHIPAWARD_H
#define STUDENTIS_FINANCE_SCHOLARSHIPAWARD_H

#include <string>

namespace studentis {

class Scholarship;
class Semester;
class Student;

/**
 * @brief Назначение стипендии студенту на семестр.
 *
 * Статусы: pending, approved, paid, cancelled.
 */
class ScholarshipAward {
public:
    ScholarshipAward() = default;
    ScholarshipAward(std::string id, Scholarship* scholarship, Student* student,
                     Semester* semester, double amount, std::string awardedAt,
                     std::string status = "pending", std::string orderNumber = "");

    /// Утвердить назначение приказом; пустой номер приказа — ошибка данных.
    void approve(std::string orderNumber);
    /// Выплатить утверждённую стипендию.
    void pay();
    /// Отменить назначение; выплаченное — ошибка данных.
    void cancel();
    /// Стипендия выплачена.
    bool isPaid() const;
    /// Назначение относится к указанному семестру.
    bool matchesSemester(const Semester& semester) const;
    /// Краткое текстовое описание назначения.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Scholarship* getScholarship() const { return scholarship_; }
    Student* getStudent() const { return student_; }
    Semester* getSemester() const { return semester_; }
    double getAmount() const { return amount_; }
    const std::string& getAwardedAt() const { return awardedAt_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getOrderNumber() const { return orderNumber_; }

private:
    std::string id_;
    Scholarship* scholarship_ = nullptr;
    Student* student_ = nullptr;
    Semester* semester_ = nullptr;
    double amount_ = 0.0;
    std::string awardedAt_;
    std::string status_ = "pending";
    std::string orderNumber_;
};

} // namespace studentis

#endif
