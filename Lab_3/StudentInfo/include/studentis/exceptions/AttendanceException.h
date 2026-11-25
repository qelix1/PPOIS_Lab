#ifndef STUDENTIS_EXCEPTIONS_ATTENDANCEEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_ATTENDANCEEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Некорректная отметка посещаемости: неизвестный статус, студент или занятие.
class AttendanceException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
