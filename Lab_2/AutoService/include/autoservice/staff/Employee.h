#ifndef AUTOSERVICE_STAFF_EMPLOYEE_H
#define AUTOSERVICE_STAFF_EMPLOYEE_H

#include <string>

namespace autoservice {

class Workshop;

/**
 * @brief Сотрудник автомастерской (базовый класс иерархии персонала).
 */
class Employee {
public:
    Employee();
    Employee(std::string id, std::string fullName, std::string phone, std::string hiredAt, int baseSalaryRub,
             std::string role, int experienceYears);

    /// Уйти в отпуск: сотрудник помечается занятым до указанной даты.
    void startVacation(const std::string& from);
    /// Выйти из отпуска.
    void endVacation(const std::string& to);
    /// Полный стаж работы в мастерской на заданный год.
    int yearsWorked(int currentYear) const;
    /// Пересмотр оклада в процентах (1..100).
    void raiseSalary(int percent);
    /// Назначить сотрудника в мастерскую.
    void assignWorkshop(Workshop& workshop);
    /// Сменить роль (например, «механик» → «мастер участка»).
    void changeRole(const std::string& newRole);
    /// Уволить: статус меняется на dismissed.
    void dismiss();

    const std::string& getId() const { return id_; }
    const std::string& getFullName() const { return fullName_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getHiredAt() const { return hiredAt_; }
    int getBaseSalaryRub() const { return baseSalaryRub_; }
    const std::string& getRole() const { return role_; }
    int getExperienceYears() const { return experienceYears_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getVacationUntil() const { return vacationUntil_; }
    Workshop* getWorkshop() const { return workshop_; }

private:
    std::string id_;
    std::string fullName_;
    std::string phone_;
    std::string hiredAt_;
    int baseSalaryRub_ = 0;
    std::string role_;
    int experienceYears_ = 0;
    std::string status_ = "active";
    std::string vacationUntil_;
    Workshop* workshop_ = nullptr;
};

} // namespace autoservice

#endif
