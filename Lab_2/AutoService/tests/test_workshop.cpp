#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "autoservice/core/Client.h"
#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/AppointmentConflictException.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/ServiceBayBusyException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/staff/Manager.h"
#include "autoservice/workshop/Appointment.h"
#include "autoservice/workshop/ClientFeedback.h"
#include "autoservice/workshop/Equipment.h"
#include "autoservice/workshop/EquipmentMaintenance.h"
#include "autoservice/workshop/MarketingCampaign.h"
#include "autoservice/workshop/SchedulePlanner.h"
#include "autoservice/workshop/ServiceBay.h"
#include "autoservice/workshop/ShopReport.h"
#include "autoservice/workshop/Workshop.h"

using namespace autoservice;

TEST_CASE("Workshop: посты и режим работы", "[workshop]") {
    Workshop workshop("WS-1", "АвтоМастер", "ул. Ленина, 10", "+79005", "09:00-18:00");
    ServiceBay b1("1", "лифтовой", 3000);
    ServiceBay b2("2", "смотровая яма", 2000);
    workshop.addBay(b1);
    workshop.addBay(b2);
    REQUIRE(workshop.getBays().size() == 2);
    REQUIRE(workshop.freeBays() == 2);
    REQUIRE(workshop.capacityPerDay() == 4);
    REQUIRE(workshop.isWorkingAt("2026-10-12 12:00"));
    REQUIRE_FALSE(workshop.isWorkingAt("2026-10-12 20:00"));
    REQUIRE_FALSE(workshop.isWorkingAt("плохая дата"));

    RepairOrder order("ORD-1", "2026-10-12", "2026-10-15");
    b1.assign(order);
    REQUIRE(workshop.freeBays() == 1);
    REQUIRE(workshop.loadFactor() == Catch::Approx(0.5));
    b1.free();
    REQUIRE(workshop.freeBays() == 2);
}

TEST_CASE("ServiceBay: занятость", "[workshop]") {
    ServiceBay bay("3", "балансировка", 2500);
    RepairOrder order("ORD-2", "2026-10-12", "2026-10-15");
    RepairOrder other("ORD-3", "2026-10-12", "2026-10-15");

    bay.assign(order);
    REQUIRE(bay.isBusy());
    REQUIRE(bay.getCurrentOrder() == &order);
    REQUIRE_FALSE(bay.canTake(other));
    REQUIRE_THROWS_AS(bay.assign(other), ServiceBayBusyException);
    bay.free();
    REQUIRE(bay.canTake(other));
    REQUIRE_THROWS_AS(bay.downtimeReport(), WorkshopException);
}

TEST_CASE("SchedulePlanner: бронирование слотов", "[workshop]") {
    SchedulePlanner planner("2026-10-15", 4, 1);
    Appointment a1("AP-1", "2026-10-15 09:00", "2026-10-15 10:00");
    Appointment a2("AP-2", "2026-10-15 10:00", "2026-10-15 11:00");
    Appointment conflict("AP-3", "2026-10-15 09:30", "2026-10-15 10:30");

    planner.book(a1);
    REQUIRE(planner.isSlotFree("2026-10-15 10:00"));
    REQUIRE_FALSE(planner.isSlotFree("2026-10-15 09:00"));
    REQUIRE_FALSE(planner.isSlotFree("2026-10-15 13:00")); // вне слотов дня
    planner.book(a2);
    REQUIRE_THROWS_AS(planner.book(conflict), AppointmentConflictException);
    REQUIRE(planner.dayLoad() == 50);

    planner.blockSlot("2026-10-15 11:00");
    planner.cancel(a1);
    planner.cancel(a2);
    planner.blockSlot("2026-10-15 09:00");
    planner.blockSlot("2026-10-15 10:00");
    REQUIRE(planner.findFreeSlot() == "2026-10-15 12:00");

    SchedulePlanner full("2026-10-16", 1, 1);
    Appointment only("AP-4", "2026-10-16 09:00", "2026-10-16 10:00");
    full.book(only);
    REQUIRE_THROWS_AS(full.findFreeSlot(), WorkshopException);
}

TEST_CASE("Appointment: жизненный цикл записи", "[workshop]") {
    Appointment appointment("AP-5", "2026-10-15 11:00", "2026-10-15 12:30");
    REQUIRE(appointment.durationMinutes() == 90);

    appointment.checkIn();
    REQUIRE(appointment.getStatus() == "checked_in");
    REQUIRE_THROWS_AS(appointment.checkIn(), InvalidOrderStatusException);
    appointment.checkOut();
    REQUIRE(appointment.getStatus() == "done");
    REQUIRE_THROWS_AS(appointment.cancel("причина"), InvalidOrderStatusException);

    Appointment other("AP-6", "2026-10-15 12:00", "2026-10-15 13:00");
    REQUIRE(appointment.conflictsWith(other));
    Appointment nextDay("AP-7", "2026-10-16 11:00", "2026-10-16 12:00");
    REQUIRE_FALSE(appointment.conflictsWith(nextDay));

    Appointment fresh("AP-8", "2026-10-15 09:00", "2026-10-15 10:00");
    fresh.reschedule("2026-10-15 15:00", "2026-10-15 16:00");
    REQUIRE(fresh.getStartsAt() == "2026-10-15 15:00");
    fresh.cancel("клиент заболел");
    REQUIRE(fresh.getStatus() == "cancelled");
    REQUIRE(fresh.getCancelReason() == "клиент заболел");
    REQUIRE_THROWS_AS(fresh.cancel("x"), InvalidOrderStatusException);
}

TEST_CASE("Equipment и его ТО", "[workshop]") {
    Equipment equipment("EQ-1", "СТЕНД-развал-схождение", "2024-01-15", "2026-12-01", 350000);
    equipment.startJob();
    REQUIRE(equipment.getStatus() == "working");
    REQUIRE_THROWS_AS(equipment.startJob(), WorkshopException);
    equipment.stop();
    REQUIRE(equipment.getStatus() == "idle");
    REQUIRE_FALSE(equipment.isCalibrationDue("2026-06-01"));
    REQUIRE(equipment.isCalibrationDue("2027-01-01"));
    equipment.decommission("износ");
    REQUIRE_THROWS_AS(equipment.startJob(), WorkshopException);
    REQUIRE_THROWS_AS(equipment.decommission(""), WorkshopException);

    EquipmentMaintenance maintenance("MT-1", "калибровка", 15000);
    maintenance.complete("2026-05-01");
    maintenance.scheduleNext("2026-11-01");
    REQUIRE_FALSE(maintenance.isOverdue("2026-06-01"));
    REQUIRE(maintenance.isOverdue("2026-12-01"));
    REQUIRE_THROWS_AS(maintenance.summarize(), WorkshopException);
}

TEST_CASE("ClientFeedback: отзывы и эскалация", "[workshop]") {
    ClientFeedback good("FB-1", 5, "Отличный сервис!", "2026-06-01");
    REQUIRE(good.isPositive());
    REQUIRE(good.toScore() == 100);
    good.respond("Спасибо за отзыв");
    REQUIRE(good.getResponse() == "Спасибо за отзыв");
    REQUIRE_THROWS_AS(good.respond(""), WorkshopException);
    Manager manager("MG-1", "Орлова", "СПб", 10);
    REQUIRE_THROWS_AS(good.escalate(manager), WorkshopException);

    ClientFeedback bad("FB-2", 1, "Долго чинили", "2026-06-02");
    REQUIRE_FALSE(bad.isPositive());
    bad.escalate(manager);
    REQUIRE(bad.getEscalatedTo() == &manager);
    REQUIRE(bad.toScore() == 20);
}

TEST_CASE("MarketingCampaign: бюджет акции", "[workshop]") {
    MarketingCampaign campaign("CMP-1", "Скидки на ТО", "2026-06-01", "2026-06-30", 100000);
    REQUIRE(campaign.isActive("2026-06-15"));
    REQUIRE_FALSE(campaign.isActive("2026-07-15"));
    REQUIRE(campaign.status("2026-05-15") == "upcoming");
    REQUIRE(campaign.status("2026-06-15") == "active");
    REQUIRE(campaign.status("2026-07-15") == "ended");

    campaign.spend(30000);
    REQUIRE(campaign.remainingBudget() == 70000);
    REQUIRE_THROWS_AS(campaign.spend(-100), InvalidAmountException);
    REQUIRE_THROWS_AS(campaign.spend(1000000), InvalidAmountException);
}

TEST_CASE("ShopReport: сводка мастерской", "[workshop]") {
    ShopReport report("RPT-1", "2026-06-01", "2026-06-30");
    report.addOrder(50000);
    report.addOrder(30000);
    REQUIRE(report.getRevenueRub() == 80000);
    REQUIRE(report.getOrdersCompleted() == 2);
    REQUIRE_THROWS_AS(report.addOrder(-1), InvalidAmountException);
    REQUIRE(report.profitMargin(60000) == Catch::Approx(25.0));
    REQUIRE(report.profitMargin(0) == Catch::Approx(100.0));

    ClientFeedback f1("FB-1", 5, "ok", "2026-06-05");
    ClientFeedback f2("FB-2", 4, "ok", "2026-06-06");
    report.addFeedback(f1);
    report.addFeedback(f2);
    REQUIRE(report.getRatingAvg() == Catch::Approx(4.5));

    report.markSigned("Директор Смирнов");
    REQUIRE(report.getSignedBy() == "Директор Смирнов");
    REQUIRE_THROWS_AS(report.markSigned(""), WorkshopException);
    REQUIRE_THROWS_AS(report.summary(), WorkshopException);
}
