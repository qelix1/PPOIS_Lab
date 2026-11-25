#ifndef STUDENTIS_ASSESSMENT_EXAMRESULT_H
#define STUDENTIS_ASSESSMENT_EXAMRESULT_H

#include <string>

namespace studentis {

class Exam;
class Student;

/**
 * @brief Результат экзамена: балл, пятибалльная оценка, ответы и допуск к пересдаче.
 */
class ExamResult {
public:
    ExamResult() = default;
    ExamResult(Exam* exam, Student* student, int score, std::string answers = "",
               std::string checkedAt = "");

    /// Экзамен сдан (не ниже 60 баллов).
    bool isPassed() const;
    /// Результат отличный (не ниже 85 баллов).
    bool isExcellent() const;
    /// Оценка по пятибалльной шкале: <60 → 2, 60..69 → 3, 70..84 → 4, >=85 → 5.
    int toFiveScale() const;
    /// Требуется пересдача.
    bool needsRetake() const;
    /// Разрешить пересдачу; для сданного экзамена — InvalidGradeException.
    void allowRetake();
    /// Краткое текстовое описание результата.
    std::string summary() const;

    Exam* getExam() const { return exam_; }
    Student* getStudent() const { return student_; }
    int getScore() const { return score_; }
    int getGrade() const { return grade_; }
    const std::string& getAnswers() const { return answers_; }
    const std::string& getCheckedAt() const { return checkedAt_; }
    bool isRetakeAllowed() const { return retakeAllowed_; }

private:
    Exam* exam_ = nullptr;
    Student* student_ = nullptr;
    int score_ = 0;
    int grade_ = 2;
    std::string answers_;
    std::string checkedAt_;
    bool retakeAllowed_ = false;
};

} // namespace studentis

#endif
