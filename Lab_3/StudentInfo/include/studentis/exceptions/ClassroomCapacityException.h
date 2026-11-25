#ifndef STUDENTIS_EXCEPTIONS_CLASSROOMCAPACITYEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_CLASSROOMCAPACITYEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Группа или мероприятие не помещается в аудиторию либо ей не хватает оборудования.
class ClassroomCapacityException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
