#ifndef AUTOSERVICE_ORDERS_DIAGNOSTIC_REPORT_H
#define AUTOSERVICE_ORDERS_DIAGNOSTIC_REPORT_H

#include <string>
#include <vector>

namespace autoservice {

class Vehicle;
class Fault;
class Mechanic;
class RepairOrder;

/**
 * @brief Заключение диагностики автомобиля: список дефектов и вердикт.
 */
class DiagnosticReport {
public:
    DiagnosticReport();
    DiagnosticReport(std::string id, std::string createdAt, int odometerKm);

    /// Добавить найденный дефект.
    void addFault(Fault& fault);
    /// Убрать дефект из отчёта.
    void removeFault(const Fault& fault);
    /// Количество критических дефектов (severity ≥ 4).
    int countCritical() const;
    /// Средняя тяжесть дефектов по шкале 1..5.
    double averageSeverity() const;
    /// Подписать заключение механиком.
    void signOff(Mechanic& mechanic);
    /// Автомобиль допустим к эксплуатации: нет критических дефектов.
    bool isRoadworthy() const;

    const std::string& getId() const { return id_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    int getOdometerKm() const { return odometerKm_; }
    const std::string& getConclusion() const { return conclusion_; }
    const std::string& getVerdict() const { return verdict_; }
    Mechanic* getAuthor() const { return author_; }
    const std::vector<Fault*>& getFaults() const { return faults_; }
    Vehicle* getVehicle() const { return vehicle_; }
    void setVehicle(Vehicle& vehicle) { vehicle_ = &vehicle; }
    void setConclusion(const std::string& text) { conclusion_ = text; }

private:
    std::string id_;
    std::string createdAt_;
    int odometerKm_ = 0;
    std::string conclusion_;
    std::string verdict_;
    Mechanic* author_ = nullptr;
    std::vector<Fault*> faults_;
    Vehicle* vehicle_ = nullptr;
};

} // namespace autoservice

#endif
