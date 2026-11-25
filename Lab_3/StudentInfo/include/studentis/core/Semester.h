#ifndef STUDENTIS_CORE_SEMESTER_H
#define STUDENTIS_CORE_SEMESTER_H

#include <string>

namespace studentis {

class AcademicYear;

/**
 * @brief Семестр: даты обучения, сессия и статус.
 */
class Semester {
public:
    Semester() = default;
    Semester(int number, AcademicYear* year, std::string startDate, std::string endDate,
             std::string examsStart, std::string examsEnd, std::string status);

    /// Попадает ли дата в интервал семестра.
    bool containsDate(const std::string& date) const;
    /// Попадает ли дата в экзаменационную сессию.
    bool isExamPeriod(const std::string& date) const;
    /// Число полных учебных недель семестра.
    int weeksCount() const;
    /// Можно ли править оценки: семестр активен и сессия не началась.
    bool canEditGrades() const;
    /// Закрыть семестр.
    void close();
    /// Семестр активен.
    bool isActive() const;
    /// Дней до начала сессии на указанную дату (0, если сессия уже идёт).
    long daysUntilExams(const std::string& date) const;

    int getNumber() const { return number_; }
    AcademicYear* getYear() const { return year_; }
    const std::string& getStartDate() const { return startDate_; }
    const std::string& getEndDate() const { return endDate_; }
    const std::string& getExamsStart() const { return examsStart_; }
    const std::string& getExamsEnd() const { return examsEnd_; }
    const std::string& getStatus() const { return status_; }

    void setYear(AcademicYear* year) { year_ = year; }
    void setStartDate(const std::string& startDate) { startDate_ = startDate; }
    void setEndDate(const std::string& endDate) { endDate_ = endDate; }
    void setExamsStart(const std::string& examsStart) { examsStart_ = examsStart; }
    void setExamsEnd(const std::string& examsEnd) { examsEnd_ = examsEnd; }
    void setStatus(const std::string& status) { status_ = status; }

private:
    int number_ = 1;
    AcademicYear* year_ = nullptr;
    std::string startDate_;
    std::string endDate_;
    std::string examsStart_;
    std::string examsEnd_;
    std::string status_ = "active";
};

} // namespace studentis

#endif
