#ifndef AUTOSERVICE_EXCEPTIONS_INVALID_ORDER_STATUS_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_INVALID_ORDER_STATUS_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Недопустимый переход статуса заказа-наряда (например, «закрыт» → «в работе»).
class InvalidOrderStatusException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
