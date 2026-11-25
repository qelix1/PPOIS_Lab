#ifndef STUDENTIS_ASSESSMENT_GRADEBOOK_H
#define STUDENTIS_ASSESSMENT_GRADEBOOK_H

#include <string>
#include <vector>

namespace studentis {

class Discipline;
class ExamResult;
class Grade;
class Semester;
class Student;

/**
 * @brief Зачётная книжка студента: оценки, результаты экзаменов и производные показатели.
 */
class GradeBook {
public:
    GradeBook() = default;
    explicit GradeBook(Student* student);

    /// Добавить оценку; дубль — no-op, повторная оценка за ту же форму контроля и семестр заменяет прежнюю.
    void addGrade(Grade& grade);
    /// Добавить результат экзамена; дубль — no-op, повторный результат по тому же экзамену заменяет прежний.
    void addExamResult(ExamResult& result);
    /// Средний балл по оценкам (0, если оценок нет).
    double average() const;
    /// Средний балл за семестр (0, если оценок нет).
    double averageForSemester(const Semester& semester) const;
    /// Есть ли неудовлетворительные оценки.
    bool hasDebts() const;
    /// Дисциплины с неудовлетворительными оценками без повторов.
    std::vector<Discipline*> debtDisciplines() const;
    /// Число отличных оценок.
    int excellentCount() const;
    /// Рейтинговый балл: средний + 2 за каждую отличную оценку − 5 за каждую задолженность, в пределах 0..100.
    double ratingScore() const;

    Student* getStudent() const { return student_; }
    const std::vector<Grade*>& getGrades() const { return grades_; }
    const std::vector<ExamResult*>& getResults() const { return results_; }

private:
    Student* student_ = nullptr;
    std::vector<Grade*> grades_;
    std::vector<ExamResult*> results_;
};

} // namespace studentis

#endif
