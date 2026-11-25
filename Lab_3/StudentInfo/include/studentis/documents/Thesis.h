#ifndef STUDENTIS_DOCUMENTS_THESIS_H
#define STUDENTIS_DOCUMENTS_THESIS_H

#include <string>

namespace studentis {

class Student;
class Teacher;

/**
 * @brief Выпускная работа: тема, руководитель, проверка на плагиат и защита.
 *
 * Статусы: draft, submitted, defended, failed. К защите допускается работа,
 * сданная с назначенной датой защиты, плагиатом не выше 20% и без статуса failed.
 */
class Thesis {
public:
    Thesis() = default;
    Thesis(std::string id, Student* student, Teacher* supervisor, std::string topic,
           std::string submittedAt, std::string defenseDate, double plagiarismPercent,
           int score, std::string status);

    /// Сдать работу на проверку.
    void submit(const std::string& submittedAt);
    /// Зафиксировать процент плагиата (0..100).
    void checkPlagiarism(double plagiarismPercent);
    /// Назначить руководителя (проверяющего) работы.
    void assignReviewer(Teacher& teacher);
    /// Работа допущена к защите.
    bool isAllowedToDefend() const;
    /// Выставить оценку; недопущенная работа — InvalidStudentDataException.
    void grade(int score);
    /// Защита успешна: статус defended и оценка не ниже 60.
    bool isPassed() const;
    /// Дней до защиты; 0 при отсутствии даты.
    int daysToDefense(const std::string& date) const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    Teacher* getSupervisor() const { return supervisor_; }
    const std::string& getTopic() const { return topic_; }
    const std::string& getSubmittedAt() const { return submittedAt_; }
    const std::string& getDefenseDate() const { return defenseDate_; }
    double getPlagiarismPercent() const { return plagiarismPercent_; }
    int getScore() const { return score_; }
    const std::string& getStatus() const { return status_; }

    void setDefenseDate(const std::string& defenseDate);
    void setTopic(const std::string& topic) { topic_ = topic; }

private:
    std::string id_;
    Student* student_ = nullptr;
    Teacher* supervisor_ = nullptr;
    std::string topic_;
    std::string submittedAt_;
    std::string defenseDate_;
    double plagiarismPercent_ = 0.0;
    int score_ = 0;
    std::string status_ = "draft";
};

} // namespace studentis

#endif
