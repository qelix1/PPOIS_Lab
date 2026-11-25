#ifndef STUDENTIS_EXCEPTIONS_STUDENTNOTFOUNDEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_STUDENTNOTFOUNDEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Обязательный поиск студента не дал результата.
class StudentNotFoundException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
