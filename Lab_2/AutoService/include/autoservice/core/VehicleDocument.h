#ifndef AUTOSERVICE_CORE_VEHICLE_DOCUMENT_H
#define AUTOSERVICE_CORE_VEHICLE_DOCUMENT_H

#include <string>

namespace autoservice {

class Vehicle;
class Client;

/**
 * @brief Документ на автомобиль (ПТС/СТС): номер, сроки, владелец.
 */
class VehicleDocument {
public:
    VehicleDocument();
    VehicleDocument(std::string docType, std::string number, std::string issuedAt, std::string expiresAt);

    /// Документ просрочен на указанную дату.
    bool isExpired(const std::string& today) const;
    /// Сколько дней осталось действовать (отрицательно — просрочен).
    long daysUntilExpiry(const std::string& today) const;
    /// Перерегистрация документа на нового владельца.
    void transferTo(Client& newOwner);
    /// Документ действителен: не просрочен и привязан к автомобилю.
    bool isValid(const std::string& today) const;

    const std::string& getNumber() const { return number_; }
    const std::string& getDocType() const { return docType_; }
    const std::string& getIssuedAt() const { return issuedAt_; }
    const std::string& getExpiresAt() const { return expiresAt_; }
    Vehicle* getVehicle() const { return vehicle_; }
    Client* getOwner() const { return owner_; }
    void linkVehicle(Vehicle& vehicle) { vehicle_ = &vehicle; }

private:
    std::string docType_;
    std::string number_;
    std::string issuedAt_;
    std::string expiresAt_;
    Vehicle* vehicle_ = nullptr;
    Client* owner_ = nullptr;
};

} // namespace autoservice

#endif
