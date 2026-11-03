#ifndef AUTOSERVICE_ORDERS_ESTIMATE_LINE_H
#define AUTOSERVICE_ORDERS_ESTIMATE_LINE_H

#include <string>

namespace autoservice {

class Part;
class Service;
class Estimate;

/**
 * @brief Строка сметы: запчасть или работа с количеством и ценой.
 */
class EstimateLine {
public:
    EstimateLine();
    EstimateLine(std::string description, int quantity, int unitPriceRub);

    /// Пересчитать сумму строки (количество × цену).
    void recalc();
    /// Строка описывает запчасть.
    bool isPartsLine() const;
    /// Строка описывает работу.
    bool isLaborLine() const;
    /// Описание строки для печати сметы.
    std::string describe() const;

    const std::string& getDescription() const { return description_; }
    int getQuantity() const { return quantity_; }
    int getUnitPriceRub() const { return unitPriceRub_; }
    int getLineTotalRub() const { return lineTotalRub_; }
    Part* getPart() const { return part_; }
    Service* getService() const { return service_; }
    void setPart(Part& part) { part_ = &part; }
    void setService(Service& service) { service_ = &service; }
    void setEstimate(Estimate& estimate) { estimate_ = &estimate; }

private:
    std::string description_;
    int quantity_ = 0;
    int unitPriceRub_ = 0;
    int lineTotalRub_ = 0;
    Part* part_ = nullptr;
    Service* service_ = nullptr;
    Estimate* estimate_ = nullptr;
};

} // namespace autoservice

#endif
