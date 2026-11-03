#ifndef AUTOSERVICE_EXCEPTIONS_WARRANTY_EXPIRED_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_WARRANTY_EXPIRED_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Гарантия на ремонт или запчасть истекла.
class WarrantyExpiredException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
