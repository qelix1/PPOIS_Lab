#ifndef AUTOSERVICE_WORKSHOP_CLIENT_FEEDBACK_H
#define AUTOSERVICE_WORKSHOP_CLIENT_FEEDBACK_H

#include <string>

namespace autoservice {

class Client;
class RepairOrder;
class Manager;

/**
 * @brief Отзыв клиента о ремонте (оценка 1..5) с эскалацией на менеджера.
 */
class ClientFeedback {
public:
    ClientFeedback();
    ClientFeedback(std::string id, int rating, std::string text, std::string createdAt);

    /// Положительный отзыв (оценка ≥ 4).
    bool isPositive() const;
    /// Ответить на отзыв от имени мастерской.
    void respond(const std::string& response);
    /// Эскалировать жалобу менеджеру (только оценки 1..2).
    void escalate(Manager& manager);
    /// Оценка в процентном виде (100 — идеально).
    int toScore() const;

    const std::string& getId() const { return id_; }
    int getRating() const { return rating_; }
    const std::string& getText() const { return text_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getResponse() const { return response_; }
    Client* getClient() const { return client_; }
    RepairOrder* getOrder() const { return order_; }
    Manager* getEscalatedTo() const { return escalatedTo_; }
    void setClient(Client& client) { client_ = &client; }
    void setOrder(RepairOrder& order) { order_ = &order; }

private:
    std::string id_;
    int rating_ = 5;
    std::string text_;
    std::string createdAt_;
    std::string response_;
    Client* client_ = nullptr;
    RepairOrder* order_ = nullptr;
    Manager* escalatedTo_ = nullptr;
};

} // namespace autoservice

#endif
