#ifndef STUDENTIS_ACADEMIC_SUBMISSION_H
#define STUDENTIS_ACADEMIC_SUBMISSION_H

#include <string>

namespace studentis {

class Assignment;
class Student;
class Teacher;

/**
 * @brief Сданная работа студента: балл, статус и проверяющий.
 */
class Submission {
public:
    Submission() = default;
    Submission(std::string id, Assignment* assignment, Student* student, std::string submittedAt,
               std::string content);

    /// Отправить работу.
    void submit();
    /// Выставить балл; вне 0..100 или выше maxScore_ — InvalidGradeException.
    void grade(int score, Teacher& checker);
    /// Работа сдана позже срока задания.
    bool isLate() const;
    /// Работа возвращена на доработку.
    bool needsRevision() const;
    /// Вернуть работу на доработку с комментарием.
    void returnForRevision(const std::string& feedback, Teacher& checker);
    /// Работа проверена.
    bool isGraded() const;
    /// Краткое текстовое описание работы.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Assignment* getAssignment() const { return assignment_; }
    Student* getStudent() const { return student_; }
    const std::string& getSubmittedAt() const { return submittedAt_; }
    const std::string& getContent() const { return content_; }
    int getScore() const { return score_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getFeedback() const { return feedback_; }
    Teacher* getCheckedBy() const { return checkedBy_; }

    void setAssignment(Assignment* assignment) { assignment_ = assignment; }
    void setContent(const std::string& content) { content_ = content; }

private:
    std::string id_;
    Assignment* assignment_ = nullptr;
    Student* student_ = nullptr;
    std::string submittedAt_;
    std::string content_;
    int score_ = -1;
    std::string status_ = "submitted";
    std::string feedback_;
    Teacher* checkedBy_ = nullptr;
};

} // namespace studentis

#endif
