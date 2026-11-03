#ifndef AUTOSERVICE_CORE_DATE_UTILS_H
#define AUTOSERVICE_CORE_DATE_UTILS_H

#include <cstdio>
#include <ctime>
#include <string>

namespace autoservice {
namespace date {

/// Текущая дата в формате ISO (YYYY-MM-DD).
inline std::string today() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_r(&t, &tm);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return std::string(buf);
}

/// Разбор даты ISO в количество дней; неверный формат → отрицательное значение.
inline long parse(const std::string& iso) {
    if (iso.size() != 10 || iso[4] != '-' || iso[7] != '-') return -1;
    std::tm tm{};
    if (std::sscanf(iso.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) return -1;
    if (tm.tm_mon < 1 || tm.tm_mon > 12 || tm.tm_mday < 1 || tm.tm_mday > 31) return -1;
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    tm.tm_hour = 12;
    std::time_t raw = std::mktime(&tm);
    if (raw == static_cast<std::time_t>(-1)) return -1;
    return static_cast<long>(raw / 86400L);
}

/// Проверка, что строка — корректная дата ISO.
inline bool isDate(const std::string& iso) {
    return parse(iso) >= 0;
}

/// Разница в днях: after − before. Неверный формат → 0.
inline long daysBetween(const std::string& before, const std::string& after) {
    long a = parse(before);
    long b = parse(after);
    if (a < 0 || b < 0) return 0;
    return b - a;
}

/// Обратное преобразование: количество дней → дата ISO.
inline std::string fromDays(long days) {
    std::time_t raw = static_cast<std::time_t>(days) * 86400L;
    std::tm* tm = std::gmtime(&raw);
    if (!tm) return "";
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    return std::string(buf);
}

/// Номер дня для строки «YYYY-MM-DD HH:MM» (или «YYYY-MM-DD»).
inline long dayNumber(const std::string& dateTime) {
    return parse(dateTime.substr(0, 10));
}

/// Минуты от полуночи для строки «YYYY-MM-DD HH:MM»; ошибка → -1.
inline int minutesOfDateTime(const std::string& dateTime) {
    std::size_t pos = dateTime.find(' ');
    if (pos == std::string::npos) return -1;
    int h = 0;
    int m = 0;
    if (std::sscanf(dateTime.c_str() + static_cast<std::ptrdiff_t>(pos) + 1, "%d:%d", &h, &m) != 2) return -1;
    if (h < 0 || h > 23 || m < 0 || m > 59) return -1;
    return h * 60 + m;
}

} // namespace date
} // namespace autoservice

#endif
