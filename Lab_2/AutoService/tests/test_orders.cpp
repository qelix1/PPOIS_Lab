#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/orders/DiagnosticReport.h"
#include "autoservice/orders/Estimate.h"
#include "autoservice/orders/EstimateLine.h"
#include "autoservice/orders/Fault.h"
#include "autoservice/orders/QualityCheck.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/orders/RepairPlan.h"
#include "autoservice/orders/Service.h"
#include "autoservice/orders/ServiceCategory.h"
#include "autoservice/orders/WorkItem.h"
#include "autoservice/staff/Manager.h"

using namespace autoservice;

TEST_CASE("RepairOrder: работы, статусы, итог", "[orders]") {
    RepairOrder order("ORD-100", "2026-03-01", "2026-03-15");
    WorkItem w1("W-1", 2, 3000);
    WorkItem w2("W-2", 3, 5000);
    order.addWork(w1);
    order.addWork(w2);
    REQUIRE(order.getWorks().size() == 2);
    REQUIRE(order.calculateTotal() == 8000);

    SECTION("переходы статусов") {
        order.changeStatus("in_progress");
        REQUIRE(order.getStatus() == "in_progress");
        REQUIRE_THROWS_AS(order.changeStatus("unknown"), InvalidOrderStatusException);
        order.changeStatus("on_hold");
        order.changeStatus("in_progress");
        order.close();
        REQUIRE(order.getStatus() == "closed");
        order.reopen();
        REQUIRE(order.getStatus() == "in_progress");
    }

    SECTION("недопустимый переход") {
        REQUIRE_THROWS_AS(order.close(), InvalidOrderStatusException);
        order.changeStatus("cancelled");
        REQUIRE_THROWS_AS(order.changeStatus("in_progress"), InvalidOrderStatusException);
    }

    SECTION("просрочка") {
        REQUIRE(order.isOverdue("2026-03-20"));
        REQUIRE_FALSE(order.isOverdue("2026-03-10"));
        order.changeStatus("cancelled");
        REQUIRE_FALSE(order.isOverdue("2026-03-20"));
    }

    SECTION("удаление работы") {
        order.removeWork(w1);
        REQUIRE(order.getWorks().size() == 1);
        REQUIRE(order.calculateTotal() == 5000);
    }
}

TEST_CASE("WorkItem: жизненный цикл", "[orders]") {
    WorkItem work("W-10", 4, 0);

    work.start();
    REQUIRE(work.getStatus() == "in_progress");
    REQUIRE_THROWS_AS(work.start(), InvalidOrderStatusException);
    work.pause("ждут запчасть");
    REQUIRE(work.getStatus() == "paused");
    work.start();
    work.complete(5);
    REQUIRE(work.getStatus() == "done");
    REQUIRE(work.getActualHours() == 5);
    REQUIRE_THROWS_AS(work.complete(3), InvalidOrderStatusException);
    REQUIRE_THROWS_AS(work.complete(0), InvalidAmountException);

    work.recalcPrice(2000);
    REQUIRE(work.getPriceRub() == 8000);
    REQUIRE(work.estimatedCost(1500) == 6000);
    REQUIRE_THROWS_AS(work.recalcPrice(0), InvalidAmountException);
    REQUIRE_THROWS_AS(work.progressReport(), WorkshopException);
}

TEST_CASE("Service и ServiceCategory: прайс-лист", "[orders]") {
    Service oil("S-OIL", "Замена масла", 2000, 2, 1, true);
    Service gen("S-GEN", "Ремонт генератора", 6000, 3, 3, false);

    REQUIRE(oil.priceFor(4) == 4000);
    REQUIRE_THROWS_AS(oil.priceFor(0), InvalidAmountException);
    REQUIRE(oil.requiresCertification(0));
    REQUIRE_FALSE(oil.requiresCertification(1));
    REQUIRE(gen.requiresCertification(2));
    REQUIRE(oil.isDue(12000, 10000));
    REQUIRE_FALSE(gen.isDue(12000, 10000));

    ServiceCategory category("CAT-1", "Электрика", "Работы по электрооборудованию");
    category.addService(gen);
    REQUIRE(category.getServiceCount() == 1);
    REQUIRE(category.findByCode("S-GEN") == &gen);
    REQUIRE(category.findByCode("S-OIL") == nullptr);
    REQUIRE(category.totalPriceFrom() == 6000);
}

TEST_CASE("Estimate и EstimateLine: смета", "[orders]") {
    Estimate estimate("EST-1", "2026-03-01");
    EstimateLine l1("Масло", 2, 1000);
    EstimateLine l2("Работа", 1, 3000);
    l1.recalc();
    l2.recalc();
    estimate.addLine(l1);
    estimate.addLine(l2);
    estimate.recalcTotal();
    REQUIRE(estimate.getTotalRub() == 5000);

    SECTION("скидка") {
        Discount discount("D10", 10, "2027-01-01", 5, 0);
        estimate.applyDiscount(discount);
        REQUIRE(estimate.getTotalRub() == 4500);
        Discount expired("DOLD", 10, "2020-01-01", 5, 0);
        REQUIRE_THROWS_AS(estimate.applyDiscount(expired), WorkshopException);
    }

    SECTION("утвердление и отклонение") {
        Manager manager("MG-1", "Орлова", "СПб", 15);
        estimate.approve(manager);
        REQUIRE(estimate.getStatus() == "approved");
        REQUIRE(estimate.getApprovedBy() == &manager);
        REQUIRE_THROWS_AS(estimate.approve(manager), InvalidOrderStatusException);
        Estimate other("EST-2", "2026-03-01");
        other.reject();
        REQUIRE(other.getStatus() == "rejected");
        REQUIRE_THROWS_AS(other.reject(), InvalidOrderStatusException);
    }

    SECTION("строки сметы") {
        Part part("SKU-1", "Масло моторное", 1000, 5, 1, "л");
        REQUIRE_FALSE(l1.isPartsLine());
        l1.setPart(part);
        REQUIRE(l1.isPartsLine());
        REQUIRE_FALSE(l1.isLaborLine());
        REQUIRE(l1.describe().find("Масло") == 0);
    }
}

TEST_CASE("DiagnosticReport и Fault: диагностика", "[orders]") {
    DiagnosticReport report("DIAG-1", "2026-03-01", 50000);
    Fault minor("F-1", "царапина", 2, "кузов", "2026-03-01");
    Fault critical("F-2", "трещина ГБЦ", 5, "двигатель", "2026-03-01");
    report.addFault(minor);
    report.addFault(critical);

    REQUIRE(report.countCritical() == 1);
    REQUIRE(report.averageSeverity() == Catch::Approx(3.5));
    REQUIRE_FALSE(report.isRoadworthy());
    REQUIRE_FALSE(minor.isCritical());
    REQUIRE(critical.isCritical());

    minor.escalate();
    REQUIRE(minor.getSeverity() == 3);
    critical.markFixed("2026-03-05");
    REQUIRE(critical.isFixed());
    REQUIRE(critical.daysOpen("2026-03-10") == 4);
}

TEST_CASE("QualityCheck и RepairPlan: ОТК и план", "[orders]") {
    QualityCheck check("QC-1", "2026-03-10", 51000);
    check.addFinding("подтёк рулевой рейки");
    REQUIRE(check.hasBlockingIssues());
    REQUIRE_THROWS_AS(check.pass(), WorkshopException);
    check.fail("не работает габарит");
    REQUIRE(check.getVerdict() == "failed");
    REQUIRE_THROWS_AS(check.report(), WorkshopException);

    RepairPlan plan("PLAN-1", 2, "2026-03-01");
    WorkItem w1("W-1", 2, 0);
    WorkItem w2("W-2", 3, 0);
    plan.addStep(w1);
    plan.addStep(w2);
    REQUIRE(plan.totalLaborHours() == 5);
    plan.reorder(0, 1);
    REQUIRE(plan.getSteps()[0] == &w2);
    REQUIRE_THROWS_AS(plan.reorder(0, 5), WorkshopException);
    Manager manager("MG-1", "Орлова", "СПб", 15);
    plan.approve(manager);
    REQUIRE(plan.getApprovedBy() == &manager);
}
