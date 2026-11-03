#ifndef AUTOSERVICE_EXCEPTIONS_INVALID_AMOUNT_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_INVALID_AMOUNT_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Отрицательная или нулевая сумма, количество очков, процентов и т. п.
class InvalidAmountException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
