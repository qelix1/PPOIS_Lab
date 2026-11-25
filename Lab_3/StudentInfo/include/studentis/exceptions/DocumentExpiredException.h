#ifndef STUDENTIS_EXCEPTIONS_DOCUMENTEXPIREDEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_DOCUMENTEXPIREDEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Использован просроченный или заблокированный документ (пропуск, справка, читательский билет).
class DocumentExpiredException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
