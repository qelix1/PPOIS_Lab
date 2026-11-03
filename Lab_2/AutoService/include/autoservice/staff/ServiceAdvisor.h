#ifndef AUTOSERVICE_STAFF_SERVICE_ADVISOR_H
#define AUTOSERVICE_STAFF_SERVICE_ADVISOR_H

#include <string>
#include <vector>

#include "autoservice/staff/Employee.h"

namespace autoservice {

class Client;
class Appointment;
class RepairOrder;

/**
 * @brief Приёмщик (service advisor): запись клиентов и сопровождение заказов.
 */
class ServiceAdvisor : public Employee {
public:
    ServiceAdvisor();
    ServiceAdvisor(std::string id, std::string fullName, std::string sector);

    /// Поставить клиента на учёт у приёмщика.
    void registerClient(Client& client);
    /// Открыть запись клиента и связать её с приёмщиком.
    void bookAppointment(Appointment& appointment);
    /// Закрыть заказ-наряд и увеличить счётчик обработанных.
    void closeOrder(RepairOrder& order);
    /// Уведомить клиента; отсутствие контактов — InvalidClientDataException.
    void notifyClient(Client& client, const std::string& message);
    /// Перенести запись клиента на новое время.
    void rescheduleAppointment(Appointment& appointment, const std::string& newStart,
                               const std::string& newEnd);
    /// Отчёт по очереди (заглушка — не реализован).
    std::string queueReport() const;

    const std::string& getSector() const { return sector_; }
    int getHandledOrders() const { return handledOrders_; }
    double getSatisfactionScore() const { return satisfactionScore_; }
    int getCurrentQueue() const { return currentQueue_; }
    std::size_t getClientsCount() const { return clients_.size(); }

private:
    std::string sector_;
    int handledOrders_ = 0;
    double satisfactionScore_ = 5.0;
    int currentQueue_ = 0;
    int clientsServed_ = 0;
    std::vector<Client*> clients_;
};

} // namespace autoservice

#endif
