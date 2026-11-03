#ifndef AUTOSERVICE_ORDERS_REPAIR_PLAN_H
#define AUTOSERVICE_ORDERS_REPAIR_PLAN_H

#include <string>
#include <vector>

namespace autoservice {

class WorkItem;
class Part;
class Manager;
class RepairOrder;

/**
 * @brief План ремонта: этапы-работы, запчасти, приоритет и утверждение.
 */
class RepairPlan {
public:
    RepairPlan();
    RepairPlan(std::string id, int priority, std::string createdAt);

    /// Добавить этап работы в план.
    void addStep(WorkItem& step);
    /// Добавить запчасть в план.
    void addPart(Part& part);
    /// Переставить этап между позициями (отсчёт с нуля).
    void reorder(int from, int to);
    /// Сумма плановых часов по всем этапам.
    int totalLaborHours() const;
    /// Утвердить план менеджером.
    void approve(Manager& manager);

    const std::string& getId() const { return id_; }
    int getPriority() const { return priority_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::vector<WorkItem*>& getSteps() const { return steps_; }
    const std::vector<Part*>& getParts() const { return parts_; }
    Manager* getApprovedBy() const { return approvedBy_; }
    void setOrder(RepairOrder& order) { order_ = &order; }

private:
    std::string id_;
    int priority_ = 0;
    std::vector<WorkItem*> steps_;
    std::vector<Part*> parts_;
    std::string createdAt_;
    Manager* approvedBy_ = nullptr;
    RepairOrder* order_ = nullptr;
};

} // namespace autoservice

#endif
