#ifndef AUTOSERVICE_STAFF_SKILL_H
#define AUTOSERVICE_STAFF_SKILL_H

#include <string>

namespace autoservice {

class Mechanic;
class Service;

/**
 * @brief Квалификация/сертификат механика (уровень допуска).
 */
class Skill {
public:
    Skill();
    Skill(std::string name, int level, std::string certifiedAt, std::string validUntil);

    /// Продлить сертификат на новую дату окончания.
    void renew(const std::string& newDate);
    /// Навык покрывает требуемый уровень допуска.
    bool meetsLevel(int requiredLevel) const;
    /// Описание навыка для карточки сотрудника.
    std::string describe() const;

    const std::string& getName() const { return name_; }
    int getLevel() const { return level_; }
    const std::string& getCertifiedAt() const { return certifiedAt_; }
    const std::string& getValidUntil() const { return validUntil_; }

private:
    std::string name_;
    int level_ = 1;
    std::string certifiedAt_;
    std::string validUntil_;
};

} // namespace autoservice

#endif
