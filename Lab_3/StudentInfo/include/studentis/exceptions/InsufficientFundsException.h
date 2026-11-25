#ifndef STUDENTIS_EXCEPTIONS_INSUFFICIENTFUNDSEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_INSUFFICIENTFUNDSEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Недопустимая денежная операция: неположительная сумма, овердрафт или переплата.
class InsufficientFundsException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
