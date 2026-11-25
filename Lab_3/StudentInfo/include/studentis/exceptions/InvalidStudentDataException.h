#ifndef STUDENTIS_EXCEPTIONS_INVALIDSTUDENTDATAEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_INVALIDSTUDENTDATAEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Некорректные данные: пустые обязательные поля, дата, статус или контакты.
class InvalidStudentDataException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
