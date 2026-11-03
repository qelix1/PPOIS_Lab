#ifndef AUTOSERVICE_EXCEPTIONS_CLIENT_NOT_FOUND_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_CLIENT_NOT_FOUND_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Клиент не найден в базе мастерской.
class ClientNotFoundException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
