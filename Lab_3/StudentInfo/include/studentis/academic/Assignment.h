#ifndef STUDENTIS_ACADEMIC_ASSIGNMENT_H
#define STUDENTIS_ACADEMIC_ASSIGNMENT_H

#include <string>
#include <vector>

namespace studentis {

class Course;
class Student;
class Submission;

/**
 * @brief Задание курса: срок, максимальный балл и сданные работы.
 */
class Assignment {
public:
    Assignment() = default;
    Assignment(std::string id, Course* course, std::string title, std::string description,
               std::string issuedAt, std::string deadline, int maxScore, std::string type);

    /// Добавить работу; дубль или повторная сдача — no-op, балл выше maxScore_ — InvalidGradeException.
    void addSubmission(Submission& submission);
    /// Найти работу студента; nullptr, если не сдана.
    Submission* submissionFor(const Student& student) const;
    /// Срок сдачи истёк на указанную дату.
    bool isOverdue(const std::string& date) const;
    /// Число проверенных работ.
    int gradedCount() const;
    /// Средний балл проверенных работ (0, если проверенных нет).
    double averageScore() const;
    /// Доля проверенных работ в процентах.
    double completionPercent() const;
    /// Продлить срок сдачи; дата раньше выдачи — ошибка данных.
    void extendDeadline(const std::string& newDeadline);
    /// Студент уже сдал работу.
    bool hasSubmission(const Student& student) const;

    const std::string& getId() const { return id_; }
    Course* getCourse() const { return course_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getDescription() const { return description_; }
    const std::string& getIssuedAt() const { return issuedAt_; }
    const std::string& getDeadline() const { return deadline_; }
    int getMaxScore() const { return maxScore_; }
    const std::string& getType() const { return type_; }
    const std::vector<Submission*>& getSubmissions() const { return submissions_; }

    void setCourse(Course* course) { course_ = course; }
    void setMaxScore(int maxScore) { maxScore_ = maxScore; }

private:
    std::string id_;
    Course* course_ = nullptr;
    std::string title_;
    std::string description_;
    std::string issuedAt_;
    std::string deadline_;
    int maxScore_ = 100;
    std::string type_;
    std::vector<Submission*> submissions_;
};

} // namespace studentis

#endif
