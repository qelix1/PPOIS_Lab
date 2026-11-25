#ifndef STUDENTIS_EXCEPTIONS_PREREQUISITENOTMETEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_PREREQUISITENOTMETEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Не пройдена дисциплина-пререквизит.
class PrerequisiteNotMetException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
