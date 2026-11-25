#ifndef STUDENTIS_EXCEPTIONS_DISCIPLINENOTFOUNDEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_DISCIPLINENOTFOUNDEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Обязательный поиск дисциплины не дал результата.
class DisciplineNotFoundException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
