#ifndef STUDENTIS_ASSESSMENT_RETAKE_H
#define STUDENTIS_ASSESSMENT_RETAKE_H

#include <string>

namespace studentis {

class ExamResult;
class Student;
class Teacher;

/**
 * @brief Пересдача экзамена: планирование, проведение и результат.
 *
 * Статусы: pending → scheduled → completed, либо cancelled.
 */
class Retake {
public:
    Retake() = default;
    Retake(std::string id, ExamResult* result, Student* student, std::string scheduledDate,
           Teacher* teacher, std::string reason);

    /// Подтвердить пересдачу; иное состояние или сданный экзамен — исключение.
    void schedule();
    /// Завершить пересдачу с новым баллом; вне 0..100 — InvalidGradeException.
    void complete(int newScore);
    /// Пересдача ожидает подтверждения.
    bool isPending() const;
    /// Пересдача возможна: экзамен не сдан и пересдача не завершена и не отменена.
    bool canRetake() const;
    /// Пересдача завершена успешно (новый балл не ниже 60).
    bool isSuccessful() const;
    /// Отменить пересдачу; завершённую пересдачу отменить нельзя.
    void cancel();

    const std::string& getId() const { return id_; }
    ExamResult* getResult() const { return result_; }
    Student* getStudent() const { return student_; }
    const std::string& getScheduledDate() const { return scheduledDate_; }
    Teacher* getTeacher() const { return teacher_; }
    const std::string& getReason() const { return reason_; }
    const std::string& getStatus() const { return status_; }
    int getNewScore() const { return newScore_; }

private:
    std::string id_;
    ExamResult* result_ = nullptr;
    Student* student_ = nullptr;
    std::string scheduledDate_;
    Teacher* teacher_ = nullptr;
    std::string reason_;
    std::string status_ = "pending";
    int newScore_ = -1;
};

} // namespace studentis

#endif
