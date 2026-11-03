#ifndef AUTOSERVICE_INVENTORY_SUPPLIER_H
#define AUTOSERVICE_INVENTORY_SUPPLIER_H

#include <string>
#include <vector>

namespace autoservice {

class Part;
class PurchaseOrder;
class ReturnOrder;

/**
 * @brief Поставщик запчастей: реквизиты, рейтинг, условия поставок.
 */
class Supplier {
public:
    Supplier();
    Supplier(std::string id, std::string name, std::string inn, std::string phone);

    /// Предложить запчасть в ассортимент поставщика.
    void addPart(Part& part);
    /// Оценить доставку (1..5 звёзд), рейтинг усредняется.
    void rateDelivery(int stars);
    /// Поставщик предпочтителен при рейтинге не ниже minRating.
    bool isPreferred(double minRating) const;
    /// Оформить заказ поставщику.
    void placeOrder(PurchaseOrder& order);
    /// Оформить возврат поставщику.
    void placeReturn(ReturnOrder& returnOrder);
    /// Качественная оценка поставщика словами.
    std::string ratingGrade() const;

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getInn() const { return inn_; }
    const std::string& getPhone() const { return phone_; }
    double getRating() const { return rating_; }
    int getLeadTimeDays() const { return leadTimeDays_; }
    const std::string& getPaymentTerms() const { return paymentTerms_; }
    const std::vector<Part*>& getParts() const { return parts_; }
    const std::vector<PurchaseOrder*>& getOrders() const { return orders_; }
    void setLeadTimeDays(int days) { leadTimeDays_ = days; }
    void setPaymentTerms(const std::string& terms) { paymentTerms_ = terms; }

private:
    std::string id_;
    std::string name_;
    std::string inn_;
    std::string phone_;
    double rating_ = 5.0;
    int leadTimeDays_ = 3;
    std::string paymentTerms_;
    std::vector<Part*> parts_;
    std::vector<PurchaseOrder*> orders_;
};

} // namespace autoservice

#endif
