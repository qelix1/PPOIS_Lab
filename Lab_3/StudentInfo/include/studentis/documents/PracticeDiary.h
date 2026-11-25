#ifndef STUDENTIS_DOCUMENTS_PRACTICEDIARY_H
#define STUDENTIS_DOCUMENTS_PRACTICEDIARY_H

#include <string>
#include <vector>

namespace studentis {

class Student;
class Teacher;

/**
 * @brief Дневник практики: организация, сроки, часы, задачи и итоговая оценка.
 */
class PracticeDiary {
public:
    PracticeDiary() = default;
    PracticeDiary(std::string id, Student* student, Teacher* supervisor, std::string organization,
                  std::string startDate, std::string endDate, int hours,
                  std::vector<std::string> tasks, int finalScore);

    /// Добавить задачу практики; пустая задача — InvalidStudentDataException.
    void addTask(const std::string& task);
    /// Трудоёмкость практики в часах.
    int totalHours() const;
    /// Практика завершена: есть задачи и итоговая оценка не ниже 60.
    bool isComplete() const;
    /// Выставить итоговую оценку (0..100).
    void rate(int score);
    /// Длительность практики в днях включительно.
    int durationDays() const;
    /// Краткое текстовое описание дневника.
    std::string summary() const;
    /// Даты корректны и начало не позже окончания.
    bool validateDates() const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    Teacher* getSupervisor() const { return supervisor_; }
    const std::string& getOrganization() const { return organization_; }
    const std::string& getStartDate() const { return startDate_; }
    const std::string& getEndDate() const { return endDate_; }
    int getHours() const { return hours_; }
    const std::vector<std::string>& getTasks() const { return tasks_; }
    int getFinalScore() const { return finalScore_; }

    void setOrganization(const std::string& organization) { organization_ = organization; }
    void setHours(int hours);

private:
    std::string id_;
    Student* student_ = nullptr;
    Teacher* supervisor_ = nullptr;
    std::string organization_;
    std::string startDate_;
    std::string endDate_;
    int hours_ = 0;
    std::vector<std::string> tasks_;
    int finalScore_ = 0;
};

} // namespace studentis

#endif
