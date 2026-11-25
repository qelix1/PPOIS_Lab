#ifndef STUDENTIS_DOCUMENTS_DIPLOMA_H
#define STUDENTIS_DOCUMENTS_DIPLOMA_H

#include <string>

namespace studentis {

class Specialty;
class Student;

/**
 * @brief Диплом: специальность, серия и номер, средний балл и тема работы.
 */
class Diploma {
public:
    Diploma() = default;
    Diploma(std::string id, Student* student, Specialty* specialty, std::string number,
            std::string series, std::string issueDate, double finalAverage, bool honors,
            std::string thesisTopic, int thesisScore);

    /// Диплом с отличием: флаг и средний балл не ниже 85.
    bool isHonors() const;
    /// Квалификация по специальности; пустая строка без специальности.
    std::string qualification() const;
    /// Диплом выдан: заполнены номер и дата выдачи.
    bool isIssued() const;
    /// Зарегистрировать выдачу диплома.
    void registerIssue(const std::string& issueDate, const std::string& number,
                       const std::string& series);
    /// Краткое текстовое описание диплома.
    std::string summary() const;
    /// Проверить формат серии (4 цифры) и номера (6 цифр).
    bool validateNumber() const;
    /// Диплом соответствует указанной специальности.
    bool matchesSpecialty(const Specialty& specialty) const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    Specialty* getSpecialty() const { return specialty_; }
    const std::string& getNumber() const { return number_; }
    const std::string& getSeries() const { return series_; }
    const std::string& getIssueDate() const { return issueDate_; }
    double getFinalAverage() const { return finalAverage_; }
    bool isHonorsFlag() const { return honors_; }
    const std::string& getThesisTopic() const { return thesisTopic_; }
    int getThesisScore() const { return thesisScore_; }

    void setSpecialty(Specialty* specialty) { specialty_ = specialty; }
    void setThesisTopic(const std::string& thesisTopic) { thesisTopic_ = thesisTopic; }
    void setThesisScore(int thesisScore);

private:
    std::string id_;
    Student* student_ = nullptr;
    Specialty* specialty_ = nullptr;
    std::string number_;
    std::string series_;
    std::string issueDate_;
    double finalAverage_ = 0.0;
    bool honors_ = false;
    std::string thesisTopic_;
    int thesisScore_ = 0;
};

} // namespace studentis

#endif
