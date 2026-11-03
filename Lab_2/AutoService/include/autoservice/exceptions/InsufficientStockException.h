#ifndef AUTOSERVICE_EXCEPTIONS_INSUFFICIENT_STOCK_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_INSUFFICIENT_STOCK_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Недостаточно запчастей на складе для резерва или списания.
class InsufficientStockException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
