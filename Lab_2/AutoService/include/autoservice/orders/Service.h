#ifndef AUTOSERVICE_ORDERS_SERVICE_H
#define AUTOSERVICE_ORDERS_SERVICE_H

#include <string>

namespace autoservice {

class ServiceCategory;

/**
 * @brief Услуга из прайс-листа: норма-часы, цена, требуемая квалификация.
 */
class Service {
public:
    Service();
    Service(std::string code, std::string name, int basePriceRub, int durationHours, int requiredSkillLevel,
            bool periodic);

    /// Стоимость услуги при заданной трудоёмкости, ч.
    int priceFor(int hours) const;
    /// Требуется ли механику уровень допуска выше указанного.
    bool requiresCertification(int mechanicSkillLevel) const;
    /// Периодическая услуга просрочена по пробегу.
    bool isDue(int sinceServiceKm, int intervalKm) const;
    /// Переименовать услугу в прайс-листе.
    void rename(const std::string& newName);
    /// Отнести услугу к категории.
    void assignCategory(ServiceCategory& category);

    const std::string& getCode() const { return code_; }
    const std::string& getName() const { return name_; }
    int getBasePriceRub() const { return basePriceRub_; }
    int getDurationHours() const { return durationHours_; }
    int getRequiredSkillLevel() const { return requiredSkillLevel_; }
    bool getPeriodic() const { return periodic_; }
    ServiceCategory* getCategory() const { return category_; }

private:
    std::string code_;
    std::string name_;
    int basePriceRub_ = 0;
    int durationHours_ = 0;
    ServiceCategory* category_ = nullptr;
    int requiredSkillLevel_ = 1;
    bool periodic_ = false;
};

} // namespace autoservice

#endif
