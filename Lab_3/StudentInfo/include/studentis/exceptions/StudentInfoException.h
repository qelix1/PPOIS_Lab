#ifndef STUDENTIS_EXCEPTIONS_STUDENTINFOEXCEPTION_H
#define STUDENTIS_EXCEPTIONS_STUDENTINFOEXCEPTION_H

#include <stdexcept>
#include <string>

namespace studentis {

/**
 * @brief Базовое исключение предметной области.
 *
 * Позволяет одним catch обработать любую ошибку информационной системы студентов.
 */
class StudentInfoException : public std::runtime_error {
public:
    explicit StudentInfoException(const std::string& message) : std::runtime_error(message) {}
};

} // namespace studentis

#endif
