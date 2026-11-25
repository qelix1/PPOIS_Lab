#include "studentis/services/NotificationService.h"

#include <algorithm>
#include <string>
#include <vector>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/services/Notification.h"

namespace studentis {

bool NotificationService::notifyStudent(Notification& notification) {
    for (const Notification* current : notifications_) {
        if (current == &notification) return current->getStatus() == "sent" || current->isRead();
    }
    const bool delivered = notification.send();
    if (delivered) {
        ++sentCount_;
    } else {
        ++failedCount_;
    }
    notifications_.push_back(&notification);
    return delivered;
}

int NotificationService::notifyGroup(StudentGroup& group,
                                     const std::vector<Notification*>& notifications) {
    int delivered = 0;
    for (Notification* notification : notifications) {
        if (notification == nullptr || notification->getRecipient() == nullptr) continue;
        Student* recipient = notification->getRecipient();
        if (!recipient->isActive()) continue;
        if (group.findStudent(recipient->getStudentId()) != recipient) continue;
        if (notifyStudent(*notification)) ++delivered;
    }
    return delivered;
}

void NotificationService::markAllRead(const std::string& date) {
    for (Notification* notification : notifications_) {
        if (notification->getStatus() == "sent") notification->markRead(date);
    }
}

std::vector<Notification*> NotificationService::unreadFor(const Student& student) const {
    std::vector<Notification*> result;
    for (Notification* notification : notifications_) {
        if (notification->isRead()) continue;
        const Student* recipient = notification->getRecipient();
        if (recipient != nullptr && recipient->getStudentId() == student.getStudentId()) {
            result.push_back(notification);
        }
    }
    return result;
}

int NotificationService::pendingCount() const {
    int count = 0;
    for (const Notification* notification : notifications_) {
        if (notification->getStatus() == "pending") ++count;
    }
    return count;
}

int NotificationService::purgeOlderThan(int days, const std::string& date) {
    if (days < 0) throw InvalidStudentDataException("порог дней не может быть отрицательным");
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата очистки уведомлений");
    const std::size_t before = notifications_.size();
    notifications_.erase(
        std::remove_if(notifications_.begin(), notifications_.end(),
                       [days, &date](Notification* notification) {
                           return notification->daysOld(date) > days;
                       }),
        notifications_.end());
    return static_cast<int>(before - notifications_.size());
}

double NotificationService::deliveryRate() const {
    const int attempts = sentCount_ + failedCount_;
    return attempts == 0 ? 0.0
                         : 100.0 * static_cast<double>(sentCount_) / static_cast<double>(attempts);
}

} // namespace studentis
