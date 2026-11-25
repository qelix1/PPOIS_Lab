#include "studentis/services/Notification.h"

#include <string>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownChannel(const std::string& channel) {
    return channel == "email" || channel == "sms" || channel == "push" || channel == "messenger";
}

bool isKnownStatus(const std::string& status) {
    return status == "pending" || status == "sent" || status == "failed" || status == "read";
}

bool isKnownPriority(const std::string& priority) {
    return priority == "low" || priority == "normal" || priority == "high";
}

} // namespace

Notification::Notification(std::string id, Student* recipient, std::string title, std::string text,
                           std::string createdAt, std::string channel, std::string status,
                           std::string priority)
    : id_(std::move(id)),
      recipient_(recipient),
      title_(std::move(title)),
      text_(std::move(text)),
      createdAt_(std::move(createdAt)),
      channel_(std::move(channel)),
      status_(std::move(status)),
      priority_(std::move(priority)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор уведомления не может быть пустым");
    if (!date::isDate(createdAt_)) throw InvalidStudentDataException("некорректная дата создания уведомления");
    if (!isKnownChannel(channel_)) throw InvalidStudentDataException("неизвестный канал уведомления: " + channel_);
    if (!isKnownStatus(status_)) throw InvalidStudentDataException("неизвестный статус уведомления: " + status_);
    if (!isKnownPriority(priority_)) throw InvalidStudentDataException("неизвестный приоритет: " + priority_);
}

void Notification::markRead(const std::string& date) {
    if (status_ == "read") return;
    if (status_ != "sent") {
        throw InvalidStudentDataException("прочитать можно только отправленное уведомление");
    }
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата прочтения");
    readAt_ = date;
    status_ = "read";
}

bool Notification::send() {
    if (status_ == "read") return true;
    if (recipient_ == nullptr || title_.empty()) {
        status_ = "failed";
        return false;
    }
    status_ = "sent";
    return true;
}

bool Notification::isRead() const {
    return status_ == "read";
}

bool Notification::isUrgent() const {
    return priority_ == "high";
}

long Notification::daysOld(const std::string& date) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата проверки уведомления");
    const long days = date::daysBetween(createdAt_, date);
    return days > 0 ? days : 0;
}

bool Notification::matchesChannel(const std::string& channel) const {
    return channel_ == channel;
}

std::string Notification::summary() const {
    std::string result = "[" + priority_ + "] " + title_;
    if (recipient_ != nullptr) result += " для " + recipient_->shortName();
    result += " (" + channel_ + ", " + status_ + ")";
    return result;
}

} // namespace studentis
