#ifndef STUDENTIS_EXCEPTIONS_GROUPFULLEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_GROUPFULLEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// В группе нет свободных мест: превышен maxSize_.
class GroupFullException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
