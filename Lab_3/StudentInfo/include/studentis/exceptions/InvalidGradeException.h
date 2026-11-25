#ifndef STUDENTIS_EXCEPTIONS_INVALIDGRADEEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_INVALIDGRADEEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Оценка вне 0..100, балл выше maxScore_ или недопустимая оценка по пятибалльной шкале.
class InvalidGradeException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
