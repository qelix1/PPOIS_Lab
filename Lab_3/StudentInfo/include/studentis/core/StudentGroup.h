#ifndef STUDENTIS_CORE_STUDENTGROUP_H
#define STUDENTIS_CORE_STUDENTGROUP_H

#include <string>
#include <vector>

namespace studentis {

class Specialty;
class Student;
class Teacher;

/**
 * @brief Учебная группа: специальность, курс, куратор и список студентов.
 */
class StudentGroup {
public:
    StudentGroup() = default;
    StudentGroup(std::string code, Specialty* specialty, int admissionYear, int courseNumber,
                 Teacher* curator, int maxSize, std::string status);

    /// Добавить студента; дубль — no-op, нет мест — GroupFullException.
    void addStudent(Student& student);
    /// Убрать студента из группы; true — если он был в группе.
    bool removeStudent(const std::string& studentId);
    /// Найти студента группы по номеру или nullptr.
    Student* findStudent(const std::string& studentId) const;
    /// Число студентов со статусом active.
    int activeCount() const;
    /// Группа заполнена по активным студентам.
    bool isFull() const;
    /// Перевести группу на следующий курс; выше шестого — ошибка данных.
    void advanceCourse();
    /// Назначить куратора группы.
    void assignCurator(Teacher& curator);
    /// Свободные места с учётом активных студентов.
    int availableSeats() const;

    /// Средний балл активных студентов группы.
    double averageScore() const;
    /// Студенты группы с академическими задолженностями.
    std::vector<Student*> studentsWithDebts() const;

    const std::string& getCode() const { return code_; }
    Specialty* getSpecialty() const { return specialty_; }
    int getAdmissionYear() const { return admissionYear_; }
    int getCourseNumber() const { return courseNumber_; }
    Teacher* getCurator() const { return curator_; }
    int getMaxSize() const { return maxSize_; }
    const std::vector<Student*>& getStudents() const { return students_; }
    const std::string& getStatus() const { return status_; }

    void setSpecialty(Specialty* specialty) { specialty_ = specialty; }
    void setCourseNumber(int courseNumber) { courseNumber_ = courseNumber; }
    void setMaxSize(int maxSize) { maxSize_ = maxSize; }
    void setStatus(const std::string& status) { status_ = status; }

private:
    std::string code_;
    Specialty* specialty_ = nullptr;
    int admissionYear_ = 0;
    int courseNumber_ = 1;
    Teacher* curator_ = nullptr;
    int maxSize_ = 25;
    std::vector<Student*> students_;
    std::string status_ = "active";
};

} // namespace studentis

#endif
