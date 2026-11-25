#ifndef STUDENTIS_SERVICES_EVENT_H
#define STUDENTIS_SERVICES_EVENT_H

#include <string>
#include <vector>

namespace studentis {

class Classroom;
class Student;
class Teacher;

/**
 * @brief Мероприятие: время, место, организатор и участники.
 *
 * Статусы: planned, ongoing, completed, cancelled.
 */
class Event {
public:
    Event() = default;
    Event(std::string id, std::string title, std::string description, std::string startAt,
          std::string endAt, Teacher* organizer, Classroom* location, int maxParticipants,
          std::string status = "planned");

    /// Зарегистрировать студента; false при дубле, заполненности, неактивном студенте или отмене.
    bool registerStudent(Student& student);
    /// Снять регистрацию; true — если студент был записан.
    bool unregister(const std::string& studentId);
    /// Отметить посещение участника; посторонний — InvalidStudentDataException.
    void markAttended(Student& student);
    /// Число участников достигло предела.
    bool isFull() const;
    /// Мероприятие ещё предстоит на указанную дату.
    bool isUpcoming(const std::string& date) const;
    /// Длительность в минутах.
    int durationMinutes() const;
    /// Доля посетивших от записавшихся в процентах.
    double attendancePercent() const;
    /// Пересечение по времени с другим мероприятием.
    bool conflictsWith(const Event& other) const;
    /// Отменить мероприятие.
    void cancel();

    const std::string& getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getDescription() const { return description_; }
    const std::string& getStartAt() const { return startAt_; }
    const std::string& getEndAt() const { return endAt_; }
    Teacher* getOrganizer() const { return organizer_; }
    Classroom* getLocation() const { return location_; }
    int getMaxParticipants() const { return maxParticipants_; }
    const std::vector<Student*>& getParticipants() const { return participants_; }
    const std::vector<Student*>& getAttended() const { return attended_; }
    const std::string& getStatus() const { return status_; }

    void setLocation(Classroom* location) { location_ = location; }
    void setMaxParticipants(int maxParticipants) { maxParticipants_ = maxParticipants; }
    void setStatus(const std::string& status) { status_ = status; }

private:
    std::string id_;
    std::string title_;
    std::string description_;
    std::string startAt_;
    std::string endAt_;
    Teacher* organizer_ = nullptr;
    Classroom* location_ = nullptr;
    int maxParticipants_ = 0;
    std::vector<Student*> participants_;
    std::vector<Student*> attended_;
    std::string status_ = "planned";
};

} // namespace studentis

#endif
