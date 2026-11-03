#include "autoservice/inventory/InventoryAudit.h"

#include <cstdlib>

#include "autoservice/inventory/InventoryLine.h"

namespace autoservice {

InventoryAudit::InventoryAudit() = default;

InventoryAudit::InventoryAudit(std::string id, std::string startedAt)
    : id_(std::move(id)), startedAt_(std::move(startedAt)) {}

void InventoryAudit::addLine(InventoryLine& line) {
    for (InventoryLine* l : lines_) {
        if (l == &line) return;
    }
    lines_.push_back(&line);
    line.setAudit(*this);
}

void InventoryAudit::finish(const std::string& date) {
    finishedAt_ = date;
    discrepancies_ = 0;
    for (const InventoryLine* l : lines_) {
        if (l && !l->isMatch()) ++discrepancies_;
    }
}

int InventoryAudit::totalDiscrepancy() const {
    int total = 0;
    for (const InventoryLine* l : lines_) {
        if (l) total += std::abs(l->diff());
    }
    return total;
}

bool InventoryAudit::hasIssues() const {
    return discrepancies_ > 0;
}

void InventoryAudit::sign(Employee& auditor) {
    auditor_ = &auditor;
}

} // namespace autoservice
