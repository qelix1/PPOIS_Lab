#ifndef STUDENTIS_SERVICES_NOTIFICATIONSERVICE_H
#define STUDENTIS_SERVICES_NOTIFICATIONSERVICE_H

#include <string>
#include <vector>

namespace studentis {

class Notification;
class Student;
class StudentGroup;

/**
 * @brief Сервис уведомлений: отправка, прочтение и очистка старых сообщений.
 *
 * Уведомления не принадлежат сервису: он хранит только указатели.
 */
class NotificationService {
public:
    NotificationService() = default;

    /// Отправить уведомление и учесть результат доставки; дубль — no-op.
    bool notifyStudent(Notification& notification);
    /// Отправить уведомления получателям из группы; вернуть число успешно доставленных.
    int notifyGroup(StudentGroup& group, const std::vector<Notification*>& notifications);
    /// Отметить прочитанными все отправленные уведомления на указанную дату.
    void markAllRead(const std::string& date);
    /// Непрочитанные уведомления студента.
    std::vector<Notification*> unreadFor(const Student& student) const;
    /// Число уведомлений, ожидающих отправки.
    int pendingCount() const;
    /// Удалить из учёта уведомления старше указанного числа дней; вернуть число удалённых.
    int purgeOlderThan(int days, const std::string& date);
    /// Доля успешных доставок в процентах (0, если отправок не было).
    double deliveryRate() const;

    const std::vector<Notification*>& getNotifications() const { return notifications_; }
    int getSentCount() const { return sentCount_; }
    int getFailedCount() const { return failedCount_; }

private:
    std::vector<Notification*> notifications_;
    int sentCount_ = 0;
    int failedCount_ = 0;
};

} // namespace studentis

#endif
