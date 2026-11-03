#ifndef AUTOSERVICE_ORDERS_SERVICE_CATEGORY_H
#define AUTOSERVICE_ORDERS_SERVICE_CATEGORY_H

#include <string>
#include <vector>

namespace autoservice {

class Service;

/**
 * @brief Категория услуг прайс-листа (ремонт двигателя, электрооборудование…).
 */
class ServiceCategory {
public:
    ServiceCategory();
    ServiceCategory(std::string id, std::string name, std::string description);

    /// Добавить услугу в категорию.
    void addService(Service& service);
    /// Убрать услугу из категории.
    void removeService(Service& service);
    /// Найти услугу по коду (nullptr — не найдена).
    Service* findByCode(const std::string& code) const;
    /// Сумма базовых цен всех услуг категории.
    int totalPriceFrom() const;

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    std::size_t getServiceCount() const { return services_.size(); }

private:
    std::string id_;
    std::string name_;
    std::string description_;
    std::vector<Service*> services_;
};

} // namespace autoservice

#endif
