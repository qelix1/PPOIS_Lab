#include "autoservice/orders/QualityCheck.h"

#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

QualityCheck::QualityCheck() = default;

QualityCheck::QualityCheck(std::string id, std::string checkedAt, int mileageAtCheck)
    : id_(std::move(id)), checkedAt_(std::move(checkedAt)), mileageAtCheck_(mileageAtCheck) {}

void QualityCheck::pass() {
    if (!findings_.empty())
        throw WorkshopException("нельзя принять ремонт при незакрытых замечаниях");
    verdict_ = "passed";
}

void QualityCheck::fail(const std::string& reason) {
    if (reason.empty()) throw WorkshopException("нужна причина провала контроля");
    findings_.push_back(reason);
    verdict_ = "failed";
}

void QualityCheck::addFinding(const std::string& item) {
    if (item.empty()) throw WorkshopException("замечание не может быть пустым");
    findings_.push_back(item);
}

bool QualityCheck::hasBlockingIssues() const {
    return verdict_ == "failed" || !findings_.empty();
}

std::string QualityCheck::report() const {
    throw WorkshopException("не реализовано: текст отчёта ОТК");
}

} // namespace autoservice
