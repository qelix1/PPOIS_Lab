#ifndef STUDENTIS_DOCUMENTS_ENROLLMENTORDER_H
#define STUDENTIS_DOCUMENTS_ENROLLMENTORDER_H

#include <string>

namespace studentis {

class Student;
class StudentGroup;
class Teacher;

/**
 * @brief Приказ о зачислении: основание, подпись и исполнение зачисления в группу.
 *
 * Статусы: draft, signed, executed, cancelled.
 */
class EnrollmentOrder {
public:
    EnrollmentOrder() = default;
    EnrollmentOrder(std::string id, Student* student, StudentGroup* group, std::string orderDate,
                    std::string orderNumber, std::string basis, Teacher* signedBy,
                    std::string status);

    /// Подписать приказ преподавателем; подписать можно только черновик.
    void sign(Teacher& teacher);
    /// Исполнить подписанный приказ: зачислить студента в группу.
    void execute();
    /// Отменить неисполненный приказ.
    void cancel();
    /// Приказ исполнен.
    bool isExecuted() const;
    /// Студент зачислен на бюджет.
    bool isBudget() const;
    /// Краткое текстовое описание приказа.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Student* getStudent() const { return student_; }
    StudentGroup* getGroup() const { return group_; }
    const std::string& getOrderDate() const { return orderDate_; }
    const std::string& getOrderNumber() const { return orderNumber_; }
    const std::string& getBasis() const { return basis_; }
    Teacher* getSignedBy() const { return signedBy_; }
    const std::string& getStatus() const { return status_; }

    void setGroup(StudentGroup* group) { group_ = group; }
    void setBasis(const std::string& basis) { basis_ = basis; }

private:
    std::string id_;
    Student* student_ = nullptr;
    StudentGroup* group_ = nullptr;
    std::string orderDate_;
    std::string orderNumber_;
    std::string basis_;
    Teacher* signedBy_ = nullptr;
    std::string status_ = "draft";
};

} // namespace studentis

#endif
