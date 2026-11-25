#include "studentis/core/Curriculum.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/StudyPlanEntry.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Curriculum::Curriculum(std::string id, Specialty* specialty, int admissionYear,
                       std::string approvedAt)
    : id_(std::move(id)),
      specialty_(specialty),
      admissionYear_(admissionYear),
      approvedAt_(std::move(approvedAt)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор учебного плана не может быть пустым");
    if (admissionYear_ < 1900) throw InvalidStudentDataException("некорректный год набора учебного плана");
    if (!approvedAt_.empty() && !date::isDate(approvedAt_)) {
        throw InvalidStudentDataException("некорректная дата утверждения плана: " + approvedAt_);
    }
}

void Curriculum::addEntry(StudyPlanEntry& entry) {
    if (entry.getSemesterNumber() < 1 || entry.getCredits() <= 0) {
        throw InvalidStudentDataException("строка плана должна иметь семестр и положительные кредиты");
    }
    if (findEntry(entry.getDiscipline(), entry.getSemesterNumber()) != nullptr) return;
    entries_.push_back(&entry);
    recalcCredits();
}

bool Curriculum::removeEntry(const Discipline* discipline, int semesterNumber) {
    for (auto it = entries_.begin(); it != entries_.end(); ++it) {
        if ((*it)->getDiscipline() == discipline && (*it)->getSemesterNumber() == semesterNumber) {
            entries_.erase(it);
            recalcCredits();
            return true;
        }
    }
    return false;
}

int Curriculum::creditsForSemester(int semesterNumber) const {
    int credits = 0;
    for (const StudyPlanEntry* entry : entries_) {
        if (entry->matchesSemester(semesterNumber)) credits += entry->getCredits();
    }
    return credits;
}

std::vector<StudyPlanEntry*> Curriculum::requiredDisciplines() const {
    std::vector<StudyPlanEntry*> result;
    for (StudyPlanEntry* entry : entries_) {
        if (entry->isMandatory()) result.push_back(entry);
    }
    return result;
}

std::vector<StudyPlanEntry*> Curriculum::electiveDisciplines() const {
    std::vector<StudyPlanEntry*> result;
    for (StudyPlanEntry* entry : entries_) {
        if (entry->isElective()) result.push_back(entry);
    }
    return result;
}

StudyPlanEntry* Curriculum::findEntry(const Discipline* discipline, int semesterNumber) const {
    for (StudyPlanEntry* entry : entries_) {
        if (entry->getDiscipline() == discipline && entry->matchesSemester(semesterNumber)) return entry;
    }
    return nullptr;
}

bool Curriculum::validate() const {
    if (entries_.empty()) return false;
    int credits = 0;
    for (const StudyPlanEntry* entry : entries_) {
        if (entry->getDiscipline() == nullptr || !entry->validateHours()) return false;
        credits += entry->getCredits();
    }
    return credits == totalCredits_;
}

int Curriculum::recalcCredits() {
    int credits = 0;
    for (const StudyPlanEntry* entry : entries_) credits += entry->getCredits();
    totalCredits_ = credits;
    return totalCredits_;
}

} // namespace studentis
