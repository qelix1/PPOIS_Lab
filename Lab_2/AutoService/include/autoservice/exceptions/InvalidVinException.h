#ifndef AUTOSERVICE_EXCEPTIONS_INVALID_VIN_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_INVALID_VIN_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// VIN не прошёл проверку (длина, символы, контрольная цифра ISO 3779).
class InvalidVinException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
