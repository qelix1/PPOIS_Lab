#include "autoservice/workshop/ClientFeedback.h"

#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/staff/Manager.h"

namespace autoservice {

ClientFeedback::ClientFeedback() = default;

ClientFeedback::ClientFeedback(std::string id, int rating, std::string text, std::string createdAt)
    : id_(std::move(id)),
      rating_(rating < 1 ? 1 : (rating > 5 ? 5 : rating)),
      text_(std::move(text)),
      createdAt_(std::move(createdAt)) {}

bool ClientFeedback::isPositive() const {
    return rating_ >= 4;
}

void ClientFeedback::respond(const std::string& response) {
    if (response.empty()) throw WorkshopException("ответ на отзыв не может быть пустым");
    response_ = response;
}

void ClientFeedback::escalate(Manager& manager) {
    if (rating_ > 2)
        throw WorkshopException("эскалация предусмотрена только для оценок 1–2");
    escalatedTo_ = &manager;
}

int ClientFeedback::toScore() const {
    return rating_ * 20;
}

} // namespace autoservice
