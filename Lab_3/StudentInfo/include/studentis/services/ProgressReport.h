#ifndef STUDENTIS_SERVICES_PROGRESSREPORT_H
#define STUDENTIS_SERVICES_PROGRESSREPORT_H

#include <string>

namespace studentis {

class AttendanceJournal;
class GradeBook;
class Semester;
class Student;

/**
 * @brief Отчёт об успеваемости студента за семестр.
 *
 * Рекомендация «нет замечаний» ставится только при среднем балле от 85,
 * отсутствии задолженностей и посещаемости от 90 процентов.
 */
class ProgressReport {
public:
    ProgressReport() = default;
    ProgressReport(std::string id, Student* student, Semester* semester);

    /// Построить отчёт по зачётной книжке и журналу посещаемости на указанную дату.
    void buildFrom(const GradeBook& gradeBook, const AttendanceJournal& journal,
                   const std::string& generatedAt);
    /// Студент успевает: средний балл не ниже 60 и нет задолженностей.
    bool isSuccessful() const;
    /// Есть ли академические задолженности.
    bool hasDebts() const;
    /// Текстовая сводка отчёта.
    std::string summaryText() const;
    /// Нужны ли меры: средний балл или посещаемость ниже 75 либо есть долги.
    bool needsImprovement() const;
    /// Отчёт лучше другого по среднему баллу, затем посещаемости, затем числу долгов.
    bool compareWith(const ProgressReport& other) const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    Semester* getSemester() const { return semester_; }
    const std::string& getGeneratedAt() const { return generatedAt_; }
    double getAverageScore() const { return averageScore_; }
    int getDebtsCount() const { return debtsCount_; }
    double getAttendancePercent() const { return attendancePercent_; }
    int getRatingPlace() const { return ratingPlace_; }
    const std::string& getRecommendations() const { return recommendations_; }

    void setRatingPlace(int ratingPlace) { ratingPlace_ = ratingPlace; }

private:
    std::string id_;
    Student* student_ = nullptr;
    Semester* semester_ = nullptr;
    std::string generatedAt_;
    double averageScore_ = 0.0;
    int debtsCount_ = 0;
    double attendancePercent_ = 0.0;
    int ratingPlace_ = 0;
    std::string recommendations_;
};

} // namespace studentis

#endif
