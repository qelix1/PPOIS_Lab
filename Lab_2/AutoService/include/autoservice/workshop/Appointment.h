#ifndef AUTOSERVICE_WORKSHOP_APPOINTMENT_H
#define AUTOSERVICE_WORKSHOP_APPOINTMENT_H

#include <string>

namespace autoservice {

class Client;
class Vehicle;
class Service;
class ServiceBay;
class ServiceAdvisor;

/**
 * @brief Запись клиента: слот времени, пост, услуга.
 *
 * Статусы: booked → checked_in → done; можно отменить.
 * Формат времени: «YYYY-MM-DD HH:MM».
 */
class Appointment {
public:
    Appointment();
    Appointment(std::string id, std::string startsAt, std::string endsAt);

    /// Перенести запись на другое время.
    void reschedule(const std::string& newStart, const std::string& newEnd);
    /// Отменить запись с причиной.
    void cancel(const std::string& reason);
    /// Клиент приехал на приём.
    void checkIn();
    /// Клиент уехал, запись завершена.
    void checkOut();
    /// Длительность записи в минутах.
    int durationMinutes() const;
    /// Пересечение интервалов с другой записью.
    bool conflictsWith(const Appointment& other) const;

    const std::string& getId() const { return id_; }
    const std::string& getStartsAt() const { return startsAt_; }
    const std::string& getEndsAt() const { return endsAt_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getCancelReason() const { return cancelReason_; }
    Client* getClient() const { return client_; }
    Vehicle* getVehicle() const { return vehicle_; }
    Service* getService() const { return service_; }
    ServiceBay* getBay() const { return bay_; }
    ServiceAdvisor* getAdvisor() const { return advisor_; }
    void setClient(Client& client) { client_ = &client; }
    void setVehicle(Vehicle& vehicle) { vehicle_ = &vehicle; }
    void setService(Service& service) { service_ = &service; }
    void setBay(ServiceBay& bay) { bay_ = &bay; }
    void setAdvisor(ServiceAdvisor& advisor) { advisor_ = &advisor; }

private:
    std::string id_;
    std::string startsAt_;
    std::string endsAt_;
    std::string status_ = "booked";
    std::string cancelReason_;
    Client* client_ = nullptr;
    Vehicle* vehicle_ = nullptr;
    Service* service_ = nullptr;
    ServiceBay* bay_ = nullptr;
    ServiceAdvisor* advisor_ = nullptr;
};

} // namespace autoservice

#endif
