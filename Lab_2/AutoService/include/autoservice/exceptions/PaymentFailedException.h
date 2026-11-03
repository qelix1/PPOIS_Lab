#ifndef AUTOSERVICE_EXCEPTIONS_PAYMENT_FAILED_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_PAYMENT_FAILED_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Операция оплаты не может быть выполнена (недостаточно средств, касса закрыта).
class PaymentFailedException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
