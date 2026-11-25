#ifndef STUDENTIS_CORE_STUDYPLANENTRY_H
#define STUDENTIS_CORE_STUDYPLANENTRY_H

#include <string>

namespace studentis {

class Discipline;

/**
 * @brief Строка учебного плана: дисциплина, семестр, часы и форма контроля.
 */
class StudyPlanEntry {
public:
    StudyPlanEntry() = default;
    StudyPlanEntry(Discipline* discipline, int semesterNumber, int credits, int lectureHours,
                   int practiceHours, int labHours, std::string controlForm, bool mandatory);

    /// Все часы строки плана.
    int totalHours() const;
    /// Форма контроля — экзамен.
    bool isExam() const;
    /// Форма контроля — зачёт.
    bool isCredit() const;
    /// Дисциплина по выбору.
    bool isElective() const;
    /// Строка относится к указанному семестру.
    bool matchesSemester(int semesterNumber) const;
    /// Часы и номер семестра корректны.
    bool validateHours() const;
    /// Сменить форму контроля; неизвестная форма — ошибка данных.
    void changeControlForm(const std::string& controlForm);

    Discipline* getDiscipline() const { return discipline_; }
    int getSemesterNumber() const { return semesterNumber_; }
    int getCredits() const { return credits_; }
    int getLectureHours() const { return lectureHours_; }
    int getPracticeHours() const { return practiceHours_; }
    int getLabHours() const { return labHours_; }
    const std::string& getControlForm() const { return controlForm_; }
    bool isMandatory() const { return mandatory_; }

    void setDiscipline(Discipline* discipline) { discipline_ = discipline; }
    void setSemesterNumber(int semesterNumber) { semesterNumber_ = semesterNumber; }
    void setCredits(int credits) { credits_ = credits; }
    void setLectureHours(int lectureHours) { lectureHours_ = lectureHours; }
    void setPracticeHours(int practiceHours) { practiceHours_ = practiceHours; }
    void setLabHours(int labHours) { labHours_ = labHours; }
    void setMandatory(bool mandatory) { mandatory_ = mandatory; }

private:
    Discipline* discipline_ = nullptr;
    int semesterNumber_ = 1;
    int credits_ = 0;
    int lectureHours_ = 0;
    int practiceHours_ = 0;
    int labHours_ = 0;
    std::string controlForm_ = "credit";
    bool mandatory_ = true;
};

} // namespace studentis

#endif
