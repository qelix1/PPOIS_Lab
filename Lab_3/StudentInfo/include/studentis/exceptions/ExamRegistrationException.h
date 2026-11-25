#ifndef STUDENTIS_EXCEPTIONS_EXAMREGISTRATIONEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_EXAMREGISTRATIONEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Некорректная регистрация на экзамен: дубль, неактивный студент, чужая группа или поздняя запись.
class ExamRegistrationException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
