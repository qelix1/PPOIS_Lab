#ifndef AUTOSERVICE_WORKSHOP_WORKSHOP_H
#define AUTOSERVICE_WORKSHOP_WORKSHOP_H

#include <string>
#include <vector>

namespace autoservice {

class ServiceBay;
class Employee;
class Appointment;
class Equipment;
class ShopReport;
class CashRegister;

/**
 * @brief Автомастерская: посты обслуживания, персонал, записи клиентов.
 */
class Workshop {
public:
    Workshop();
    Workshop(std::string id, std::string name, std::string address, std::string phone,
             std::string workingHours);

    /// Добавить пост обслуживания.
    void addBay(ServiceBay& bay);
    /// Принять сотрудника в штат.
    void hire(Employee& employee);
    /// Количество свободных постов.
    int freeBays() const;
    /// Загрузка мастерской 0..1 (доля занятых постов).
    double loadFactor() const;
    /// Открыта ли мастерская в указанную дату-время (по графику workingHours).
    bool isWorkingAt(const std::string& dateTime) const;
    /// Сколько заказов в день вмещает мастерская.
    int capacityPerDay() const;
    /// Записать клиента на пост.
    void scheduleAppointment(Appointment& appointment);

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getAddress() const { return address_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getWorkingHours() const { return workingHours_; }
    const std::vector<ServiceBay*>& getBays() const { return bays_; }
    const std::vector<Employee*>& getStaff() const { return staff_; }
    const std::vector<Appointment*>& getAppointments() const { return appointments_; }
    void setCashRegister(CashRegister& cash) { cashRegister_ = &cash; }
    CashRegister* getCashRegister() const { return cashRegister_; }

private:
    std::string id_;
    std::string name_;
    std::string address_;
    std::string phone_;
    std::string workingHours_;
    std::vector<ServiceBay*> bays_;
    std::vector<Employee*> staff_;
    std::vector<Appointment*> appointments_;
    CashRegister* cashRegister_ = nullptr;
};

} // namespace autoservice

#endif
