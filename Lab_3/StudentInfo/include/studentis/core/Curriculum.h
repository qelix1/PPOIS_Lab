#ifndef STUDENTIS_CORE_CURRICULUM_H
#define STUDENTIS_CORE_CURRICULUM_H

#include <string>
#include <vector>

namespace studentis {

class Discipline;
class Specialty;
class StudyPlanEntry;

/**
 * @brief Учебный план специальности: дисциплины по семестрам и зачётные единицы.
 */
class Curriculum {
public:
    Curriculum() = default;
    Curriculum(std::string id, Specialty* specialty, int admissionYear, std::string approvedAt);

    /// Добавить строку плана; дубль (дисциплина и семестр) — no-op.
    void addEntry(StudyPlanEntry& entry);
    /// Убрать строку плана; true — если она была в плане.
    bool removeEntry(const Discipline* discipline, int semesterNumber);
    /// Зачётные единицы за семестр.
    int creditsForSemester(int semesterNumber) const;
    /// Обязательные строки плана.
    std::vector<StudyPlanEntry*> requiredDisciplines() const;
    /// Дисциплины по выбору.
    std::vector<StudyPlanEntry*> electiveDisciplines() const;
    /// Найти строку плана по дисциплине и семестру; nullptr, если нет.
    StudyPlanEntry* findEntry(const Discipline* discipline, int semesterNumber) const;
    /// План корректен: есть строки, часы валидны, кредиты сходятся.
    bool validate() const;
    /// Пересчитать totalCredits_ по строкам плана и вернуть новое значение.
    int recalcCredits();

    const std::string& getId() const { return id_; }
    Specialty* getSpecialty() const { return specialty_; }
    int getAdmissionYear() const { return admissionYear_; }
    const std::string& getApprovedAt() const { return approvedAt_; }
    int getTotalCredits() const { return totalCredits_; }
    const std::vector<StudyPlanEntry*>& getEntries() const { return entries_; }

    void setSpecialty(Specialty* specialty) { specialty_ = specialty; }
    void setApprovedAt(const std::string& approvedAt) { approvedAt_ = approvedAt; }

private:
    std::string id_;
    Specialty* specialty_ = nullptr;
    int admissionYear_ = 0;
    std::string approvedAt_;
    int totalCredits_ = 0;
    std::vector<StudyPlanEntry*> entries_;
};

} // namespace studentis

#endif
