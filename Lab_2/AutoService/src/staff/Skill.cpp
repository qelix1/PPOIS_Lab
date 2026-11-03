#include "autoservice/staff/Skill.h"

namespace autoservice {

Skill::Skill() = default;

Skill::Skill(std::string name, int level, std::string certifiedAt, std::string validUntil)
    : name_(std::move(name)), level_(level), certifiedAt_(std::move(certifiedAt)), validUntil_(std::move(validUntil)) {}

void Skill::renew(const std::string& newDate) {
    validUntil_ = newDate;
}

bool Skill::meetsLevel(int requiredLevel) const {
    return level_ >= requiredLevel;
}

std::string Skill::describe() const {
    return name_ + " (уровень " + std::to_string(level_) + ", до " + validUntil_ + ")";
}

} // namespace autoservice
