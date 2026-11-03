#ifndef AUTOSERVICE_EXCEPTIONS_SERVICE_BAY_BUSY_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_SERVICE_BAY_BUSY_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Пост обслуживания занят другим заказом-нарядом.
class ServiceBayBusyException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
