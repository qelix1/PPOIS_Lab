#ifndef AUTOSERVICE_EXCEPTIONS_MECHANIC_NOT_AVAILABLE_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_MECHANIC_NOT_AVAILABLE_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Механик недоступен: перегружен, в отпуске или не имеет нужной квалификации.
class MechanicNotAvailableException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
