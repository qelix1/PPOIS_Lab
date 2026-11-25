#ifndef STUDENTIS_CORE_STUDENT_H
#define STUDENTIS_CORE_STUDENT_H

#include <string>
#include <vector>

#include "studentis/core/Person.h"

namespace studentis {

class AcademicLeave;
class Discipline;
class Exam;
class Grade;
class StudentGroup;

/**
 * @brief Студент: группа, курс, форма обучения, оценки и академический статус.
 *
 * Статусы: active, academic_leave, expelled, graduated. Только active студент
 * может сдавать экзамены, посещать мероприятия и получать стипендию.
 */
class Student : public Person {
public:
    Student() = default;
    Student(std::string id, std::string lastName, std::string firstName, std::string middleName,
            std::string birthDate, std::string gender, std::string phone, std::string email,
            std::string address, std::string studentId, std::string enrolledAt, std::string status,
            std::string educationForm, std::string funding, int courseNumber, std::string notes = "");

    /// Привязать студента к группе и синхронизировать номер курса.
    void assignGroup(StudentGroup& group);
    /// Перевести в другую группу; отсутствие цели — GroupNotFoundException, нет мест — GroupFullException.
    void transferTo(StudentGroup* target);
    /// Отчислить студента и убрать его из группы.
    void expel();
    /// Восстановить отчисленного студента; иной статус — ошибка данных.
    void restore();
    /// Перевести на следующий курс; выше шестого — ошибка данных.
    void advanceCourse();
    /// Студент обучается (статус active).
    bool isActive() const;
    /// Студент учится за счёт бюджета.
    bool isBudget() const;

    /// Записать оценку в зачётную книжку.
    void recordGrade(Grade& grade);
    /// Оценки студента по дисциплине.
    std::vector<Grade*> gradesForDiscipline(const Discipline& discipline) const;
    /// Средний балл по всем оценкам (0..100).
    double averageGrade() const;
    /// Есть ли академическая задолженность.
    bool hasAcademicDebt() const;
    /// Допущен ли студент к экзамену по дисциплине.
    bool canTakeExam(const Exam& exam) const;
    /// Оформить академический отпуск; при успехе статус academic_leave.
    AcademicLeave requestAcademicLeave(const std::string& startDate, const std::string& endDate,
                                       const std::string& reason);

    const std::string& getStudentId() const { return studentId_; }
    StudentGroup* getGroup() const { return group_; }
    const std::string& getEnrolledAt() const { return enrolledAt_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getEducationForm() const { return educationForm_; }
    const std::string& getFunding() const { return funding_; }
    int getCourseNumber() const { return courseNumber_; }
    const std::vector<Grade*>& getGrades() const { return grades_; }
    const std::string& getNotes() const { return notes_; }

    void setStatus(const std::string& status);
    void setEducationForm(const std::string& educationForm) { educationForm_ = educationForm; }
    void setFunding(const std::string& funding) { funding_ = funding; }
    void setCourseNumber(int courseNumber);
    void setNotes(const std::string& notes) { notes_ = notes; }

private:
    std::string studentId_;
    StudentGroup* group_ = nullptr;
    std::string enrolledAt_;
    std::string status_ = "active";
    std::string educationForm_;
    std::string funding_;
    int courseNumber_ = 1;
    std::vector<Grade*> grades_;
    std::string notes_;
};

} // namespace studentis

#endif
