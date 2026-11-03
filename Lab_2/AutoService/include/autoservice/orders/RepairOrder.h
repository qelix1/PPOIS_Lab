#ifndef AUTOSERVICE_ORDERS_REPAIR_ORDER_H
#define AUTOSERVICE_ORDERS_REPAIR_ORDER_H

#include <string>
#include <vector>

namespace autoservice {

class Client;
class Vehicle;
class ServiceAdvisor;
class Mechanic;
class WorkItem;
class Estimate;
class Invoice;
class DiagnosticReport;

/**
 * @brief Заказ-наряд: сердце мастерской — работы, исполнители, смета и счёт.
 *
 * Статусы: new → in_progress → on_hold/closed/cancelled; закрытый можно вернуть
 * в работу методом reopen().
 */
class RepairOrder {
public:
    RepairOrder();
    RepairOrder(std::string number, std::string createdAt, std::string deadlineAt);

    /// Добавить работу в заказ.
    void addWork(WorkItem& work);
    /// Убрать работу из заказа.
    void removeWork(const WorkItem& work);
    /// Суммарная стоимость всех работ заказа.
    int calculateTotal() const;
    /// Перевести заказ в следующий статус (недопустимый переход — исключение).
    void changeStatus(const std::string& next);
    /// Просрочен ли заказ на указанную дату.
    bool isOverdue(const std::string& today) const;
    /// Назначить механика.
    void assignMechanic(Mechanic& mechanic);
    /// Прикрепить смету.
    void attachEstimate(Estimate& estimate);
    /// Закрыть заказ (должен быть в работе).
    void close();
    /// Возобновить закрытый заказ.
    void reopen();

    const std::string& getNumber() const { return number_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getDeadlineAt() const { return deadlineAt_; }
    const std::string& getStatus() const { return status_; }
    Client* getClient() const { return client_; }
    Vehicle* getVehicle() const { return vehicle_; }
    ServiceAdvisor* getAdvisor() const { return advisor_; }
    Mechanic* getMechanic() const { return mechanic_; }
    const std::vector<WorkItem*>& getWorks() const { return works_; }
    Estimate* getEstimate() const { return estimate_; }
    Invoice* getInvoice() const { return invoice_; }
    DiagnosticReport* getDiagnostic() const { return diagnostic_; }
    void setClient(Client& client) { client_ = &client; }
    void setVehicle(Vehicle& vehicle) { vehicle_ = &vehicle; }
    void setAdvisor(ServiceAdvisor& advisor) { advisor_ = &advisor; }
    void setInvoice(Invoice& invoice) { invoice_ = &invoice; }
    void setDiagnostic(DiagnosticReport& diagnostic) { diagnostic_ = &diagnostic; }

private:
    std::string number_;
    std::string createdAt_;
    std::string deadlineAt_;
    std::string status_ = "new";
    Client* client_ = nullptr;
    Vehicle* vehicle_ = nullptr;
    ServiceAdvisor* advisor_ = nullptr;
    Mechanic* mechanic_ = nullptr;
    std::vector<WorkItem*> works_;
    Estimate* estimate_ = nullptr;
    Invoice* invoice_ = nullptr;
    DiagnosticReport* diagnostic_ = nullptr;
};

} // namespace autoservice

#endif
