#ifndef STUDENTIS_EXCEPTIONS_GROUPNOTFOUNDEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_GROUPNOTFOUNDEXCEPTION_H

#include "studentis/exceptions/StudentInfoException.h"

namespace studentis {

/// Обязательный поиск группы не дал результата либо цель перевода отсутствует.
class GroupNotFoundException : public StudentInfoException {
public:
    using StudentInfoException::StudentInfoException;
};

} // namespace studentis

#endif
