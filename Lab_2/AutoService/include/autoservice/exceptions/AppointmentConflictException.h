#ifndef AUTOSERVICE_EXCEPTIONS_APPOINTMENT_CONFLICT_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_APPOINTMENT_CONFLICT_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Запись клиента конфликтует с уже занятым слотом расписания.
class AppointmentConflictException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
