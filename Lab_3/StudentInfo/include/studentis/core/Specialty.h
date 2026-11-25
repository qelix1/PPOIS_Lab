#ifndef STUDENTIS_CORE_SPECIALTY_H
#define STUDENTIS_CORE_SPECIALTY_H

#include <string>

namespace studentis {

class Curriculum;
class Faculty;

/**
 * @brief Специальность: квалификация, срок обучения, стоимость и бюджетные места.
 *
 * tuitionFee_ — стоимость обучения за весь срок; annualFee() делит её на studyYears_.
 */
class Specialty {
public:
    Specialty() = default;
    Specialty(std::string code, std::string name, Faculty* faculty, std::string qualification,
              int studyYears, double tuitionFee, int budgetSeats);

    /// Программа длинная (6 лет и более).
    bool isLongProgram() const;
    /// Стоимость одного года обучения.
    double annualFee() const;
    /// Есть ли бюджетные места.
    bool hasBudgetSeats() const;
    /// Срок обучения в месяцах (10 учебных месяцев в году).
    int durationMonths() const;
    /// Совпадение кода специальности без учёта регистра.
    bool matchesCode(const std::string& code) const;
    /// Создать учебный план для указанного года набора.
    Curriculum createCurriculum(std::string id, int admissionYear);
    /// Переименовать специальность; пустое название — ошибка данных.
    void rename(const std::string& name);

    const std::string& getCode() const { return code_; }
    const std::string& getName() const { return name_; }
    Faculty* getFaculty() const { return faculty_; }
    const std::string& getQualification() const { return qualification_; }
    int getStudyYears() const { return studyYears_; }
    double getTuitionFee() const { return tuitionFee_; }
    int getBudgetSeats() const { return budgetSeats_; }

    void setFaculty(Faculty* faculty) { faculty_ = faculty; }
    void setStudyYears(int studyYears) { studyYears_ = studyYears; }
    void setTuitionFee(double tuitionFee) { tuitionFee_ = tuitionFee; }
    void setBudgetSeats(int budgetSeats) { budgetSeats_ = budgetSeats; }

private:
    std::string code_;
    std::string name_;
    Faculty* faculty_ = nullptr;
    std::string qualification_;
    int studyYears_ = 4;
    double tuitionFee_ = 0.0;
    int budgetSeats_ = 0;
};

} // namespace studentis

#endif
