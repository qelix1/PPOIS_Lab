#ifndef AUTOSERVICE_ORDERS_WORK_ITEM_H
#define AUTOSERVICE_ORDERS_WORK_ITEM_H

#include <string>

namespace autoservice {

class Service;
class Mechanic;
class Fault;
class RepairOrder;

/**
 * @brief Отдельная работа внутри заказа-наряда (этап ремонта).
 */
class WorkItem {
public:
    WorkItem();
    WorkItem(std::string id, int plannedHours, int priceRub);

    /// Начать выполнение работы (только из статуса new).
    void start();
    /// Завершить работу, указав фактические часы.
    void complete(int hoursWorked);
    /// Приостановить выполняемую работу с указанием причины.
    void pause(const std::string& reason);
    /// Пересчитать цену по ставке руб/час.
    void recalcPrice(int laborRateRubPerHour);
    /// Оценка стоимости по ставке без выполнения.
    int estimatedCost(int laborRateRubPerHour) const;
    /// Отчёт о ходе выполнения (заглушка — не реализован).
    std::string progressReport() const;

    const std::string& getId() const { return id_; }
    Service* getService() const { return service_; }
    Mechanic* getMechanic() const { return mechanic_; }
    int getPlannedHours() const { return plannedHours_; }
    int getActualHours() const { return actualHours_; }
    const std::string& getStatus() const { return status_; }
    int getPriceRub() const { return priceRub_; }
    Fault* getFault() const { return fault_; }
    void setService(Service& service) { service_ = &service; }
    void setMechanic(Mechanic& mechanic) { mechanic_ = &mechanic; }
    void setPrice(int rub) { priceRub_ = rub; }
    void setFault(Fault& fault) { fault_ = &fault; }

private:
    std::string id_;
    Service* service_ = nullptr;
    Mechanic* mechanic_ = nullptr;
    int plannedHours_ = 0;
    int actualHours_ = 0;
    std::string status_ = "new";
    int priceRub_ = 0;
    Fault* fault_ = nullptr;
};

} // namespace autoservice

#endif
