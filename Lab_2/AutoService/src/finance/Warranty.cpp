#include "autoservice/finance/Warranty.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/exceptions/WarrantyExpiredException.h"

namespace autoservice {

Warranty::Warranty() = default;

Warranty::Warranty(std::string id, std::string startsAt, std::string endsAt, int termsKm)
    : id_(std::move(id)), startsAt_(std::move(startsAt)), endsAt_(std::move(endsAt)), termsKm_(termsKm) {}

bool Warranty::isExpired(const std::string& today) const {
    return date::daysBetween(endsAt_, today) > 0;
}

long Warranty::daysLeft(const std::string& today) const {
    return date::daysBetween(today, endsAt_);
}

bool Warranty::coversPart(const Part& part) const {
    for (const Part* p : coveredParts_) {
        if (p == &part) return true;
    }
    return false;
}

void Warranty::claim(RepairOrder& order) {
    if (isExpired(date::today()))
        throw WarrantyExpiredException("гарантия " + id_ + " истекла");
    if (status_ != "active")
        throw WorkshopException("гарантийный случай уже открыт, статус: " + status_);
    order_ = &order;
    status_ = "claimed";
}

void Warranty::extend(int months) {
    if (months <= 0) throw InvalidAmountException("продление гарантии должно быть в месяцах (>0)");
    long endDays = date::parse(endsAt_);
    if (endDays < 0) throw WorkshopException("некорректная дата окончания гарантии");
    endsAt_ = date::fromDays(endDays + static_cast<long>(months) * 30L);
}

} // namespace autoservice
