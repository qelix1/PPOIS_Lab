#ifndef STUDENTIS_ACADEMIC_TEACHINGLOAD_H
#define STUDENTIS_ACADEMIC_TEACHINGLOAD_H

#include <string>
#include <vector>

namespace studentis {

class Course;
class Semester;
class Teacher;

/**
 * @brief Нагрузка преподавателя в семестре: план, проведённые часы и курсы.
 */
class TeachingLoad {
public:
    TeachingLoad() = default;
    TeachingLoad(Teacher* teacher, Semester* semester, int plannedHours, int conductedHours = 0);

    /// Добавить курс; дубль — no-op.
    void addCourse(Course& course);
    /// Учесть проведённые часы; неположительное значение — ошибка данных.
    void registerConductedHours(int hours);
    /// Остаток плановых часов (может быть отрицательным).
    int remainingHours() const;
    /// Часы сверх плана.
    int overloadHours() const;
    /// Процент выполнения плана.
    double completionPercent() const;
    /// Нагрузка превышена.
    bool isOverloaded() const;

    Teacher* getTeacher() const { return teacher_; }
    Semester* getSemester() const { return semester_; }
    int getPlannedHours() const { return plannedHours_; }
    int getConductedHours() const { return conductedHours_; }
    const std::vector<Course*>& getCourses() const { return courses_; }

    void setTeacher(Teacher* teacher) { teacher_ = teacher; }
    void setSemester(Semester* semester) { semester_ = semester; }
    void setPlannedHours(int plannedHours) { plannedHours_ = plannedHours; }

private:
    Teacher* teacher_ = nullptr;
    Semester* semester_ = nullptr;
    int plannedHours_ = 0;
    int conductedHours_ = 0;
    std::vector<Course*> courses_;
};

} // namespace studentis

#endif
