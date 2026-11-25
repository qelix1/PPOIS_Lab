#ifndef STUDENTIS_EXCEPTIONS_DORMITORYFULLEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_DORMITORYFULLEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Комната общежития переполнена или не соответствует полу студента.
class DormitoryFullException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
