#ifndef STUDENTIS_DOCUMENTS_TRANSFERAPPLICATION_H
#define STUDENTIS_DOCUMENTS_TRANSFERAPPLICATION_H

#include <string>

namespace studentis {

class Student;
class StudentGroup;
class Teacher;

/**
 * @brief Заявление о переводе: исходная и целевая группы, решение и исполнение.
 *
 * Статусы: pending, approved, rejected, applied.
 */
class TransferApplication {
public:
    TransferApplication() = default;
    TransferApplication(std::string id, Student* student, StudentGroup* fromGroup,
                        StudentGroup* toGroup, std::string reason, std::string createdAt,
                        std::string status, Teacher* decidedBy, std::string decisionDate);

    /// Утвердить заявление; решение фиксируется преподавателем и датой.
    void approve(Teacher& teacher, const std::string& decisionDate);
    /// Отклонить заявление.
    void reject(Teacher& teacher, const std::string& decisionDate);
    /// Заявление ожидает решения.
    bool isPending() const;
    /// Заявление корректно: студент и разные группы заданы, студент в исходной группе.
    bool isValid() const;
    /// Исполнить утверждённое заявление: перевести студента в целевую группу.
    void apply();
    /// Краткое текстовое описание заявления.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    StudentGroup* getFromGroup() const { return fromGroup_; }
    StudentGroup* getToGroup() const { return toGroup_; }
    const std::string& getReason() const { return reason_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getStatus() const { return status_; }
    Teacher* getDecidedBy() const { return decidedBy_; }
    const std::string& getDecisionDate() const { return decisionDate_; }

    void setReason(const std::string& reason) { reason_ = reason; }
    void setToGroup(StudentGroup* toGroup) { toGroup_ = toGroup; }

private:
    std::string id_;
    Student* student_ = nullptr;
    StudentGroup* fromGroup_ = nullptr;
    StudentGroup* toGroup_ = nullptr;
    std::string reason_;
    std::string createdAt_;
    std::string status_ = "pending";
    Teacher* decidedBy_ = nullptr;
    std::string decisionDate_;
};

} // namespace studentis

#endif
