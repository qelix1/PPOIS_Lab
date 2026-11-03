#ifndef AUTOSERVICE_EXCEPTIONS_VEHICLE_NOT_FOUND_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_VEHICLE_NOT_FOUND_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Автомобиль не найден в базе мастерской.
class VehicleNotFoundException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
