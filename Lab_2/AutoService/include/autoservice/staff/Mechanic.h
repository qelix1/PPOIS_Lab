#ifndef AUTOSERVICE_STAFF_MECHANIC_H
#define AUTOSERVICE_STAFF_MECHANIC_H

#include <vector>

#include "autoservice/staff/Employee.h"

namespace autoservice {

class RepairOrder;
class WorkItem;
class Skill;
class Service;

/**
 * @brief Механик: специализация, квалификация, загрузка ремонтами.
 */
class Mechanic : public Employee {
public:
    Mechanic();
    Mechanic(std::string id, std::string fullName, int experienceYears, std::string specialization);

    /// Принять заказ-наряд в работу; перегрузка — MechanicNotAvailableException.
    void acceptOrder(RepairOrder& order);
    /// Завершить работу над этапом и снять часы с загрузки.
    void completeWork(WorkItem& work);
    /// Добавить сертификацию (навык).
    void addCertification(Skill& skill);
    /// Механик перегружен сверх maxHours в неделю.
    bool isOverloaded(int maxHours) const;
    /// Средняя трудоёмкость выполненных заказов, ч (заглушка — нет учёта).
    double averageRepairTime() const;
    /// Допущен ли механик к работе по услуге (уровень квалификации).
    bool canPerform(const Service& service) const;

    const std::string& getSpecialization() const { return specialization_; }
    int getCompletedOrders() const { return completedOrders_; }
    double getProductivityScore() const { return productivityScore_; }
    double getRating() const { return rating_; }
    int getCurrentLoadHours() const { return currentLoadHours_; }
    const std::vector<Skill*>& getCertifications() const { return certifications_; }

private:
    std::string specialization_;
    int completedOrders_ = 0;
    double productivityScore_ = 1.0;
    std::vector<Skill*> certifications_;
    int currentLoadHours_ = 0;
    double rating_ = 5.0;
};

} // namespace autoservice

#endif
