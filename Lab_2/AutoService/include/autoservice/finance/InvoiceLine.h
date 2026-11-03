#ifndef AUTOSERVICE_FINANCE_INVOICE_LINE_H
#define AUTOSERVICE_FINANCE_INVOICE_LINE_H

#include <string>

namespace autoservice {

class Part;
class WorkItem;
class Invoice;

/**
 * @brief Строка счёта: запчасть или работа.
 */
class InvoiceLine {
public:
    InvoiceLine();
    InvoiceLine(std::string description, int quantity, int unitPriceRub);

    /// Пересчитать сумму строки.
    void recalc();
    /// Строка о запчасти.
    bool isPartsLine() const;
    /// Строка о работе.
    bool isLaborLine() const;
    /// Описание строки для печати счёта.
    std::string describe() const;

    const std::string& getDescription() const { return description_; }
    int getQuantity() const { return quantity_; }
    int getUnitPriceRub() const { return unitPriceRub_; }
    int getTotalRub() const { return totalRub_; }
    Part* getPart() const { return part_; }
    WorkItem* getWork() const { return work_; }
    Invoice* getInvoice() const { return invoice_; }
    void setPart(Part& part) { part_ = &part; }
    void setWork(WorkItem& work) { work_ = &work; }
    void setInvoice(Invoice& invoice) { invoice_ = &invoice; }

private:
    std::string description_;
    int quantity_ = 0;
    int unitPriceRub_ = 0;
    int totalRub_ = 0;
    Part* part_ = nullptr;
    WorkItem* work_ = nullptr;
    Invoice* invoice_ = nullptr;
};

} // namespace autoservice

#endif
