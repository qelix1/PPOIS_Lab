#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/MechanicNotAvailableException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/orders/Service.h"
#include "autoservice/orders/WorkItem.h"
#include "autoservice/staff/Director.h"
#include "autoservice/staff/Employee.h"
#include "autoservice/staff/Manager.h"
#include "autoservice/staff/Mechanic.h"
#include "autoservice/staff/Salary.h"
#include "autoservice/staff/Shift.h"
#include "autoservice/staff/Skill.h"
#include "autoservice/staff/WorkSchedule.h"

using namespace autoservice;

TEST_CASE("Employee: оклад, отпуск, увольнение", "[staff]") {
    Employee employee("E-1", "Иванов И.И.", "+79001", "2020-03-15", 50000, "Механик", 5);

    SECTION("повышение оклада") {
        employee.raiseSalary(10);
        REQUIRE(employee.getBaseSalaryRub() == 55000);
        REQUIRE_THROWS_AS(employee.raiseSalary(0), InvalidAmountException);
        REQUIRE_THROWS_AS(employee.raiseSalary(150), InvalidAmountException);
    }

    SECTION("отпуск") {
        employee.startVacation("2026-08-01");
        REQUIRE(employee.getStatus() == "on_vacation");
        REQUIRE_THROWS_AS(employee.startVacation("2026-09-01"), WorkshopException);
        employee.endVacation("2026-08-15");
        REQUIRE(employee.getStatus() == "active");
    }

    SECTION("стаж и увольнение") {
        REQUIRE(employee.yearsWorked(2026) == 6);
        employee.dismiss();
        REQUIRE(employee.getStatus() == "dismissed");
    }
}

TEST_CASE("Mechanic: приём заказов и загрузка", "[staff]") {
    Mechanic mechanic("M-1", "Петров П.П.", 8, "Подвеска");
    RepairOrder order("ORD-1", "2026-01-10", "2026-01-20");

    mechanic.acceptOrder(order);
    REQUIRE(order.getMechanic() == &mechanic);

    WorkItem work("W-1", 5, 0);
    mechanic.completeWork(work);
    REQUIRE(work.getStatus() == "done");
    REQUIRE(mechanic.getCompletedOrders() == 1);

    std::vector<WorkItem> works;
    works.reserve(6);
    for (int i = 0; i < 6; ++i) works.emplace_back("WI" + std::to_string(i), 8, 0);
    std::vector<RepairOrder> extras;
    extras.reserve(6);
    for (int i = 0; i < 6; ++i) {
        extras.emplace_back("ORD-X" + std::to_string(i), "2026-01-10", "2026-01-20");
        extras.back().addWork(works[i]);
        mechanic.acceptOrder(extras.back());
    }
    REQUIRE(mechanic.getCurrentLoadHours() == 48);
    REQUIRE(mechanic.isOverloaded(40));
    RepairOrder oneMore("ORD-XT", "2026-01-10", "2026-01-20");
    REQUIRE_THROWS_AS(mechanic.acceptOrder(oneMore), MechanicNotAvailableException);

    Skill skill("Подвеска", 3, "2024-01-01", "2027-01-01");
    mechanic.addCertification(skill);
    Service easy("S1", "Свеча", 500, 1, 2, false);
    Service hard("S2", "Ремонт ГРМ", 15000, 6, 5, false);
    REQUIRE(mechanic.canPerform(easy));
    REQUIRE_FALSE(mechanic.canPerform(hard));
}

TEST_CASE("Salary: расчёт лист", "[staff]") {
    Employee employee("E-2", "Сидоров", "", "2024-01-01", 40000, "Приёмщик", 2);
    Salary salary(40000, "2026-01-01", "2026-01-31", &employee);

    salary.accrueBonus(5000);
    salary.applyPenalty(1000);
    REQUIRE(salary.netPay() == 44000);
    REQUIRE(salary.isPayDay("2026-01-31"));
    REQUIRE_FALSE(salary.isPayDay("2026-01-15"));

    REQUIRE_THROWS_AS(salary.accrueBonus(-1), InvalidAmountException);
    REQUIRE_THROWS_AS(salary.applyPenalty(100000), InvalidAmountException);
}

TEST_CASE("Shift и WorkSchedule: график", "[staff]") {
    Employee employee("E-3", "Кузнецов", "", "2025-01-01", 30000, "Механик", 1);
    Shift morning("2026-01-12 09:00", "2026-01-12 18:00", &employee, nullptr);
    Shift evening("2026-01-12 17:00", "2026-01-12 21:00", &employee, nullptr);

    REQUIRE(morning.durationMinutes() == 540);
    REQUIRE(morning.overlapsWith(evening));
    REQUIRE_FALSE(morning.isFinished("2026-01-12 12:00"));
    REQUIRE(morning.isFinished("2026-01-12 19:00"));

    WorkSchedule schedule(2, "2026-01-05", 1);
    schedule.addShift(morning);
    REQUIRE(schedule.getShiftCount() == 1);
    REQUIRE_FALSE(schedule.isEmployeeFree(employee, "2026-01-12"));
    REQUIRE(schedule.isEmployeeFree(employee, "2026-01-13"));
    REQUIRE(schedule.hoursPerWeek(employee) == 9);
    schedule.removeShift(morning);
    REQUIRE(schedule.getShiftCount() == 0);
}

TEST_CASE("Manager и Director: полномочия", "[staff]") {
    Manager manager("MG-1", "Орлова", "СПб", 10);
    Discount small("S5", 5, "2027-01-01", 10, 0);
    Discount big("S30", 30, "2027-01-01", 10, 0);
    REQUIRE_NOTHROW(manager.approveDiscount(small));
    REQUIRE_THROWS_AS(manager.approveDiscount(big), InvalidAmountException);

    Mechanic mechanic("M-2", "Волков", 3, "Электрика");
    RepairOrder order("ORD-2", "2026-02-01", "2026-02-10");
    manager.assignMechanic(order, mechanic);
    REQUIRE(order.getMechanic() == &mechanic);

    std::vector<Mechanic*> team{&mechanic};
    REQUIRE(manager.teamProductivity(team) == Catch::Approx(1.0));
    REQUIRE(manager.teamProductivity({}) == 0.0);

    Director director("D-1", "Смирнов", "АвтоПрофи");
    director.approveBudget(1000000);
    REQUIRE(director.getBudgetRub() == 1000000);
    REQUIRE_THROWS_AS(director.approveBudget(-1), InvalidAmountException);
    director.dismissStaff(mechanic);
    REQUIRE(mechanic.getStatus() == "dismissed");
}

TEST_CASE("Skill: уровни допуска", "[staff]") {
    Skill skill("Диагностика", 2, "2025-05-01", "2026-05-01");
    REQUIRE(skill.meetsLevel(2));
    REQUIRE_FALSE(skill.meetsLevel(3));
    skill.renew("2028-05-01");
    REQUIRE(skill.getValidUntil() == "2028-05-01");
    REQUIRE(skill.describe().find("Диагностика") == 0);
}
