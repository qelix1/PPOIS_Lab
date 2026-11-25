#ifndef STUDENTIS_ACADEMIC_LESSON_H
#define STUDENTIS_ACADEMIC_LESSON_H

#include <string>

namespace studentis {

class Classroom;
class Course;

/**
 * @brief Занятие: дата, время, тип, аудитория и тема.
 */
class Lesson {
public:
    Lesson() = default;
    Lesson(std::string id, Course* course, std::string date, std::string startTime,
           std::string endTime, std::string type, Classroom* classroom, std::string topic,
           bool conducted = false);

    /// Длительность занятия в минутах.
    int durationMinutes() const;
    /// Наложение занятий: та же дата и пересечение интервалов.
    bool overlaps(const Lesson& other) const;
    /// Перенести занятие; неверный интервал — InvalidStudentDataException.
    void moveTo(const std::string& date, const std::string& startTime, const std::string& endTime);
    /// Назначить аудиторию; не подходит — ClassroomCapacityException.
    void assignClassroom(Classroom& classroom);
    /// Отметить занятие как проведённое.
    void markConducted();
    /// Занятие является лекцией.
    bool isLecture() const;
    /// Занятие приходится на указанную дату.
    bool isOn(const std::string& date) const;
    /// Краткое текстовое описание занятия.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Course* getCourse() const { return course_; }
    const std::string& getDate() const { return date_; }
    const std::string& getStartTime() const { return startTime_; }
    const std::string& getEndTime() const { return endTime_; }
    const std::string& getType() const { return type_; }
    Classroom* getClassroom() const { return classroom_; }
    const std::string& getTopic() const { return topic_; }
    bool isConducted() const { return conducted_; }

    void setCourse(Course* course) { course_ = course; }
    void setTopic(const std::string& topic) { topic_ = topic; }

private:
    std::string id_;
    Course* course_ = nullptr;
    std::string date_;
    std::string startTime_;
    std::string endTime_;
    std::string type_;
    Classroom* classroom_ = nullptr;
    std::string topic_;
    bool conducted_ = false;
};

} // namespace studentis

#endif
