#include "autoservice/orders/DiagnosticReport.h"

#include "autoservice/orders/Fault.h"

namespace autoservice {

DiagnosticReport::DiagnosticReport() = default;

DiagnosticReport::DiagnosticReport(std::string id, std::string createdAt, int odometerKm)
    : id_(std::move(id)), createdAt_(std::move(createdAt)), odometerKm_(odometerKm) {}

void DiagnosticReport::addFault(Fault& fault) {
    for (Fault* f : faults_) {
        if (f == &fault) return;
    }
    faults_.push_back(&fault);
}

void DiagnosticReport::removeFault(const Fault& fault) {
    for (auto it = faults_.begin(); it != faults_.end(); ++it) {
        if (*it == &fault) {
            faults_.erase(it);
            return;
        }
    }
}

int DiagnosticReport::countCritical() const {
    int count = 0;
    for (const Fault* f : faults_) {
        if (f && f->isCritical()) ++count;
    }
    return count;
}

double DiagnosticReport::averageSeverity() const {
    if (faults_.empty()) return 0.0;
    int sum = 0;
    int count = 0;
    for (const Fault* f : faults_) {
        if (f) {
            sum += f->getSeverity();
            ++count;
        }
    }
    return count == 0 ? 0.0 : static_cast<double>(sum) / static_cast<double>(count);
}

void DiagnosticReport::signOff(Mechanic& mechanic) {
    author_ = &mechanic;
    verdict_ = "signed";
}

bool DiagnosticReport::isRoadworthy() const {
    return countCritical() == 0;
}

} // namespace autoservice
