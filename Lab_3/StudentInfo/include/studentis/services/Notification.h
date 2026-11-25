#ifndef STUDENTIS_SERVICES_NOTIFICATION_H
#define STUDENTIS_SERVICES_NOTIFICATION_H

#include <string>

namespace studentis {

class Student;

/**
 * @brief Уведомление студенту: канал доставки, приоритет и статус прочтения.
 *
 * Каналы: email, sms, push, messenger. Статусы: pending, sent, failed, read.
 * Приоритеты: low, normal, high.
 */
class Notification {
public:
    Notification() = default;
    Notification(std::string id, Student* recipient, std::string title, std::string text,
                 std::string createdAt, std::string channel, std::string status = "pending",
                 std::string priority = "normal");

    /// Отметить прочитанным; непрочитанное неотправленное — InvalidStudentDataException.
    void markRead(const std::string& date);
    /// Отправить; false — нет получателя или пустой заголовок.
    bool send();
    /// Уведомление прочитано.
    bool isRead() const;
    /// Высокий приоритет.
    bool isUrgent() const;
    /// Число полных дней с момента создания на указанную дату.
    long daysOld(const std::string& date) const;
    /// Канал доставки совпадает с запросом.
    bool matchesChannel(const std::string& channel) const;
    /// Краткое текстовое описание уведомления.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Student* getRecipient() const { return recipient_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getText() const { return text_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getChannel() const { return channel_; }
    const std::string& getStatus() const { return status_; }
    const std::string& getReadAt() const { return readAt_; }
    const std::string& getPriority() const { return priority_; }

    void setRecipient(Student* recipient) { recipient_ = recipient; }
    void setText(const std::string& text) { text_ = text; }
    void setStatus(const std::string& status) { status_ = status; }
    void setPriority(const std::string& priority) { priority_ = priority; }

private:
    std::string id_;
    Student* recipient_ = nullptr;
    std::string title_;
    std::string text_;
    std::string createdAt_;
    std::string channel_;
    std::string status_ = "pending";
    std::string readAt_;
    std::string priority_ = "normal";
};

} // namespace studentis

#endif
