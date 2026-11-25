#ifndef STUDENTIS_EXCEPTIONS_SCHEDULECONFLICTEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_SCHEDULECONFLICTEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Наложение занятий по времени у группы, преподавателя или аудитории.
class ScheduleConflictException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
