#ifndef AUTOSERVICE_EXCEPTIONS_WORKSHOP_EXCEPTION_H
#define AUTOSERVICE_EXCEPTIONS_WORKSHOP_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace autoservice {

/**
 * @brief Базовое исключение предметной области «Автомастерская».
 *
 * Все собственные исключения проекта наследуются от него,
 * что позволяет перехватывать ошибки мастерской одним блоком catch.
 */
class WorkshopException : public std::runtime_error {
public:
    explicit WorkshopException(const std::string& message) : std::runtime_error(message) {}
};

} // namespace autoservice

#endif
