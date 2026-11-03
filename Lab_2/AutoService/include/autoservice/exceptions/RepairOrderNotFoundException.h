#ifndef AUTOSERVICE_EXCEPTIONS_REPAIR_ORDER_NOT_FOUND_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_REPAIR_ORDER_NOT_FOUND_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Заказ-наряд не найден.
class RepairOrderNotFoundException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
