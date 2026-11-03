#include "autoservice/inventory/Supplier.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/inventory/PurchaseOrder.h"
#include "autoservice/inventory/ReturnOrder.h"

namespace autoservice {

Supplier::Supplier() = default;

Supplier::Supplier(std::string id, std::string name, std::string inn, std::string phone)
    : id_(std::move(id)), name_(std::move(name)), inn_(std::move(inn)), phone_(std::move(phone)) {}

void Supplier::addPart(Part& part) {
    for (Part* p : parts_) {
        if (p == &part) return;
    }
    parts_.push_back(&part);
    part.setSupplier(*this);
}

void Supplier::rateDelivery(int stars) {
    if (stars < 1 || stars > 5) throw InvalidAmountException("оценка доставки должна быть от 1 до 5");
    rating_ = (rating_ + static_cast<double>(stars)) / 2.0;
}

bool Supplier::isPreferred(double minRating) const {
    return rating_ >= minRating;
}

void Supplier::placeOrder(PurchaseOrder& order) {
    if (order.getSupplier() != nullptr && order.getSupplier() != this)
        throw WorkshopException("заказ уже привязан к другому поставщику");
    order.setSupplier(*this);
    for (PurchaseOrder* o : orders_) {
        if (o == &order) return;
    }
    orders_.push_back(&order);
}

void Supplier::placeReturn(ReturnOrder& returnOrder) {
    returnOrder.setSupplier(*this);
}

std::string Supplier::ratingGrade() const {
    if (rating_ >= 4.5) return "отлично";
    if (rating_ >= 4.0) return "хорошо";
    if (rating_ >= 3.0) return "удовлетворительно";
    return "неудовлетворительно";
}

} // namespace autoservice
