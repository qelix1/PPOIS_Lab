#ifndef AUTOSERVICE_EXCEPTIONS_INVALID_CLIENT_DATA_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_INVALID_CLIENT_DATA_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Некорректные данные клиента (пустое имя, неверный телефон и т. п.).
class InvalidClientDataException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
