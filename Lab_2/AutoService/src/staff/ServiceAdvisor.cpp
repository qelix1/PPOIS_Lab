#include "autoservice/staff/ServiceAdvisor.h"

#include "autoservice/core/Client.h"
#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidClientDataException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/workshop/Appointment.h"

namespace autoservice {

ServiceAdvisor::ServiceAdvisor() = default;

ServiceAdvisor::ServiceAdvisor(std::string id, std::string fullName, std::string sector)
    : Employee(std::move(id), std::move(fullName), "", date::today(), 50000, "Приёмщик", 0),
      sector_(std::move(sector)) {}

void ServiceAdvisor::registerClient(Client& client) {
    for (Client* c : clients_) {
        if (c == &client) return;
    }
    clients_.push_back(&client);
    ++clientsServed_;
}

void ServiceAdvisor::bookAppointment(Appointment& appointment) {
    appointment.setAdvisor(*this);
    ++currentQueue_;
}

void ServiceAdvisor::closeOrder(RepairOrder& order) {
    order.changeStatus("closed");
    ++handledOrders_;
    if (currentQueue_ > 0) --currentQueue_;
}

void ServiceAdvisor::notifyClient(Client& client, const std::string& message) {
    if (message.empty()) throw WorkshopException("текст уведомления не может быть пустым");
    if (client.getPhone().empty() && client.getEmail().empty())
        throw InvalidClientDataException("у клиента нет контактов для уведомления");
}

void ServiceAdvisor::rescheduleAppointment(Appointment& appointment, const std::string& newStart,
                                           const std::string& newEnd) {
    appointment.reschedule(newStart, newEnd);
}

std::string ServiceAdvisor::queueReport() const {
    throw WorkshopException("не реализовано: отчёт по очереди приёмки");
}

} // namespace autoservice
