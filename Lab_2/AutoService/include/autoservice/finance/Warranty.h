#ifndef AUTOSERVICE_FINANCE_WARRANTY_H
#define AUTOSERVICE_FINANCE_WARRANTY_H

#include <string>
#include <vector>

namespace autoservice {

class RepairOrder;
class Part;

/**
 * @brief Гарантия на выполненный ремонт и/или запчасти.
 *
 * Статусы: active → claimed.
 */
class Warranty {
public:
    Warranty();
    Warranty(std::string id, std::string startsAt, std::string endsAt, int termsKm);

    /// Гарантия истекла на указанную дату.
    bool isExpired(const std::string& today) const;
    /// Сколько дней гарантии осталось (отрицательно — истекла).
    long daysLeft(const std::string& today) const;
    /// Покрывается ли запчасть гарантией.
    bool coversPart(const Part& part) const;
    /// Открыть гарантийный случай по заказу; истёкшая гарантия — исключение.
    void claim(RepairOrder& order);
    /// Продлить гарантию на количество месяцев.
    void extend(int months);

    const std::string& getId() const { return id_; }
    const std::string& getStartsAt() const { return startsAt_; }
    const std::string& getEndsAt() const { return endsAt_; }
    int getTermsKm() const { return termsKm_; }
    const std::string& getStatus() const { return status_; }
    RepairOrder* getOrder() const { return order_; }
    const std::vector<Part*>& getCoveredParts() const { return coveredParts_; }
    void coverPart(Part& part) { coveredParts_.push_back(&part); }

private:
    std::string id_;
    std::string startsAt_;
    std::string endsAt_;
    int termsKm_ = 0;
    std::string status_ = "active";
    RepairOrder* order_ = nullptr;
    std::vector<Part*> coveredParts_;
};

} // namespace autoservice

#endif
