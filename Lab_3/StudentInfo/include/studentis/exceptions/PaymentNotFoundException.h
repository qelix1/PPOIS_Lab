#ifndef STUDENTIS_EXCEPTIONS_PAYMENTNOTFOUNDEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_PAYMENTNOTFOUNDEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Обязательный поиск или регистрация платежа ссылается на неизвестный платёж.
class PaymentNotFoundException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
