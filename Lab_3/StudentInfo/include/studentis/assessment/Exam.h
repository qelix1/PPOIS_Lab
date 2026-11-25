#ifndef STUDENTIS_ASSESSMENT_EXAM_H
#define STUDENTIS_ASSESSMENT_EXAM_H

#include <string>
#include <vector>

namespace studentis {

class Classroom;
class Discipline;
class ExamTicket;
class Student;
class StudentGroup;
class Teacher;

/**
 * @brief Экзамен: дисциплина, группа, преподаватель, дата и билеты.
 *
 * Статусы: scheduled, in_progress, completed, cancelled. Регистрация студентов
 * открыта только для запланированного экзамена.
 */
class Exam {
public:
    Exam() = default;
    Exam(std::string id, Discipline* discipline, StudentGroup* group, Teacher* teacher,
         std::string date, Classroom* classroom, int maxScore,
         std::string status = "scheduled");

    /// Добавить билет; повторный номер или чужой экзамен не принимаются.
    void addTicket(ExamTicket& ticket);
    /// Убрать билет по номеру; true — если билет был.
    bool removeTicket(int number);
    /// Экзамен назначен на указанную дату.
    bool isScheduledOn(const std::string& date) const;
    /// Экзамен можно начать: статус scheduled и есть хотя бы один билет.
    bool canStart() const;
    /// Зарегистрировать студента; дубль, неактивный, чужая группа или закрытая запись — ExamRegistrationException.
    void registerStudent(Student& student);
    /// Число зарегистрированных студентов.
    int registeredCount() const;
    /// Завершить экзамен; повторное завершение или отмена — InvalidStudentDataException.
    void complete();
    /// Краткое текстовое описание экзамена.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Discipline* getDiscipline() const { return discipline_; }
    StudentGroup* getGroup() const { return group_; }
    Teacher* getTeacher() const { return teacher_; }
    const std::string& getDate() const { return date_; }
    Classroom* getClassroom() const { return classroom_; }
    int getMaxScore() const { return maxScore_; }
    const std::vector<ExamTicket*>& getTickets() const { return tickets_; }
    const std::string& getStatus() const { return status_; }

    void setClassroom(Classroom* classroom) { classroom_ = classroom; }

private:
    std::string id_;
    Discipline* discipline_ = nullptr;
    StudentGroup* group_ = nullptr;
    Teacher* teacher_ = nullptr;
    std::string date_;
    Classroom* classroom_ = nullptr;
    int maxScore_ = 100;
    std::vector<ExamTicket*> tickets_;
    std::string status_ = "scheduled";
    // Приватный помощник: список записей нужен для контроля дублей и допуска.
    std::vector<Student*> registered_;
};

} // namespace studentis

#endif
