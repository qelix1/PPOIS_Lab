#ifndef AUTOSERVICE_INVENTORY_INVENTORY_LINE_H
#define AUTOSERVICE_INVENTORY_INVENTORY_LINE_H

#include <string>

namespace autoservice {

class Part;
class InventoryAudit;

/**
 * @brief Строка инвентаризации: ожидаемый и фактический остаток запчасти.
 */
class InventoryLine {
public:
    InventoryLine();
    InventoryLine(Part* part, int expectedQty, int actualQty);

    /// Отклонение: факт − ожидание.
    int diff() const;
    /// Учёт сошёлся.
    bool isMatch() const;
    /// Описание расхождения для акта.
    std::string describe() const;

    Part* getPart() const { return part_; }
    int getExpectedQty() const { return expectedQty_; }
    int getActualQty() const { return actualQty_; }
    const std::string& getComment() const { return comment_; }
    InventoryAudit* getAudit() const { return audit_; }
    void setAudit(InventoryAudit& audit) { audit_ = &audit; }
    void setComment(const std::string& comment) { comment_ = comment; }

private:
    Part* part_ = nullptr;
    int expectedQty_ = 0;
    int actualQty_ = 0;
    std::string comment_;
    InventoryAudit* audit_ = nullptr;
};

} // namespace autoservice

#endif
