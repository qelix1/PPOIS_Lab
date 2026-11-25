#ifndef STUDENTIS_ASSESSMENT_GRADE_H
#define STUDENTIS_ASSESSMENT_GRADE_H

#include <string>

namespace studentis {

class Discipline;
class Semester;
class Student;
class Teacher;

/**
 * @brief Оценка студента по дисциплине в стобалльной шкале.
 *
 * Сданной считается оценка от 60, отличной — от 85, неудовлетворительной — ниже 60.
 */
class Grade {
public:
    Grade() = default;
    Grade(std::string id, Student* student, Discipline* discipline, Teacher* teacher, int value,
          std::string date, std::string controlForm, Semester* semester = nullptr,
          std::string comment = "");

    /// Оценка сдана (не ниже 60 баллов).
    bool isPass() const;
    /// Оценка отличная (не ниже 85 баллов).
    bool isExcellent() const;
    /// Оценка неудовлетворительная (ниже 60 баллов).
    bool isFail() const;
    /// Изменить балл; вне 0..100 — InvalidGradeException.
    void updateValue(int value);
    /// Оценка выставлена за указанный семестр.
    bool matchesSemester(const Semester& semester) const;
    /// Вес оценки при расчёте среднего: экзамен 1.0, дифференцированный зачёт 0.8, зачёт 0.5.
    double weight() const;
    /// Краткое текстовое описание оценки.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    Discipline* getDiscipline() const { return discipline_; }
    Teacher* getTeacher() const { return teacher_; }
    int getValue() const { return value_; }
    const std::string& getDate() const { return date_; }
    const std::string& getControlForm() const { return controlForm_; }
    Semester* getSemester() const { return semester_; }
    const std::string& getComment() const { return comment_; }

    void setTeacher(Teacher* teacher) { teacher_ = teacher; }
    void setSemester(Semester* semester) { semester_ = semester; }
    void setComment(const std::string& comment) { comment_ = comment; }

private:
    std::string id_;
    Student* student_ = nullptr;
    Discipline* discipline_ = nullptr;
    Teacher* teacher_ = nullptr;
    int value_ = 0;
    std::string date_;
    std::string controlForm_;
    Semester* semester_ = nullptr;
    std::string comment_;
};

} // namespace studentis

#endif
