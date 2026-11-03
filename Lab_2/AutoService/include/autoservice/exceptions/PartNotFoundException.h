#ifndef AUTOSERVICE_EXCEPTIONS_PART_NOT_FOUND_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_PART_NOT_FOUND_EXCEPTION_H

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

/// Запчасть не найдена на складе или в каталоге.
class PartNotFoundException : public WorkshopException {
public:
    using WorkshopException::WorkshopException;
};

} // namespace autoservice

#endif
