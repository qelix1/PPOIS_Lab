#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "autoservice/core/CarBrand.h"
#include "autoservice/core/CarModel.h"
#include "autoservice/core/Client.h"
#include "autoservice/core/DateUtils.h"
#include "autoservice/core/Engine.h"
#include "autoservice/core/Transmission.h"
#include "autoservice/core/Vehicle.h"
#include "autoservice/core/VehicleDocument.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidClientDataException.h"
#include "autoservice/exceptions/InvalidVinException.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/finance/LoyaltyProgram.h"

using namespace autoservice;

TEST_CASE("Client: бонусы и лояльность", "[core]") {
    Client client("C-1", "Иванов Иван", "+79000000001", "i@i.ru");

    SECTION("начисление и списание") {
        client.addBonus(300);
        client.spendBonus(100);
        REQUIRE(client.getBonusPoints() == 200);
    }

    SECTION("некорректные бонусы") {
        REQUIRE_THROWS_AS(client.addBonus(-5), InvalidAmountException);
        REQUIRE_THROWS_AS(client.spendBonus(0), InvalidAmountException);
        REQUIRE_THROWS_AS(client.spendBonus(1), InvalidAmountException);
    }

    SECTION("контакты") {
        REQUIRE_THROWS_AS(client.updateContact("", ""), InvalidClientDataException);
        client.updateContact("+79000000002", "");
        REQUIRE(client.getPhone() == "+79000000002");
    }

    SECTION("лояльность и скидка") {
        client.addBonus(500);
        REQUIRE(client.isLoyal(100));
        Discount discount("D1", 10, "2027-12-31", 10, 0);
        REQUIRE(client.canUseDiscount(discount, "2026-01-15"));
        Discount expired("D2", 10, "2020-01-01", 10, 0);
        REQUIRE_FALSE(client.canUseDiscount(expired, "2026-01-15"));
    }
}

TEST_CASE("Client: уровни программы лояльности", "[core]") {
    Client client("C-2", "Петров Пётр", "+79000000003", "p@p.ru");
    LoyaltyProgram program("Bonus", 1.0, 100, 0);
    program.addTier(0, "базовый");
    program.addTier(300, "серебро");
    program.addTier(700, "золото");
    REQUIRE(client.loyaltyTier(program) == "базовый");
    client.addBonus(350);
    REQUIRE(client.loyaltyTier(program) == "серебро");
    client.addBonus(400);
    REQUIRE(client.loyaltyTier(program) == "золото");
}

TEST_CASE("Vehicle: пробег и техобслуживание", "[core]") {
    Vehicle vehicle("1M8GDM9AXKP042788", 2019, "Серый");

    SECTION("корректный VIN проходит проверку") {
        REQUIRE_NOTHROW(vehicle.validateVin());
    }

    SECTION("некорректные VIN") {
        Vehicle shortVin("123", 2020, "Белый");
        REQUIRE_THROWS_AS(shortVin.validateVin(), InvalidVinException);
        Vehicle badChar("1M8GDM9AIKP042788", 2020, "Белый");
        REQUIRE_THROWS_AS(badChar.validateVin(), InvalidVinException);
        Vehicle badDigit("1M8GDM9A0KP042788", 2020, "Белый");
        REQUIRE_THROWS_AS(badDigit.validateVin(), InvalidVinException);
    }

    SECTION("пробег") {
        vehicle.increaseMileage(5000);
        REQUIRE(vehicle.getMileageKm() == 5000);
        REQUIRE_THROWS_AS(vehicle.increaseMileage(-10), InvalidAmountException);
        REQUIRE(vehicle.needsServiceByMileage(4000));
        REQUIRE_FALSE(vehicle.needsServiceByMileage(6000));
        REQUIRE(vehicle.averageMonthlyMileage(5) == 1000.0);
        REQUIRE_THROWS_AS(vehicle.averageMonthlyMileage(0), InvalidAmountException);
    }

    SECTION("владелец и агрегаты") {
        Client owner("C-3", "Сидоров С.С.", "+79000000004", "s@s.ru");
        vehicle.assignOwner(owner);
        REQUIRE(vehicle.getOwner() == &owner);
        REQUIRE(owner.getVehicleCount() == 1);
        Engine engine("2GR-FE", "бензин", 3.5, 249, 320, "АИ-95");
        Transmission gearbox("AT", 6, "передний", "Aisin", 60000);
        vehicle.attachEngine(engine);
        vehicle.attachTransmission(gearbox);
        REQUIRE(vehicle.getEngine() == &engine);
        REQUIRE(vehicle.getTransmission() == &gearbox);
    }
}

TEST_CASE("Engine и Transmission: характеристики", "[core]") {
    Engine engine("EA888", "Turbo бензин", 2.0, 190, 320, "АИ-95");
    REQUIRE(engine.isTurbo());
    REQUIRE(engine.powerPerLiter() == Catch::Approx(95.0));
    REQUIRE(engine.needsOilChange(9000, 8000));
    REQUIRE_FALSE(engine.needsOilChange(7000, 8000));

    Transmission manual("MT", 6, "передний", "Getrag", 60000);
    REQUIRE_FALSE(manual.isAutomatic());
    REQUIRE(manual.expectedLifespanKm() == 400000);
    Transmission autoBox("AT", 6, "полный", "Aisin", 60000);
    REQUIRE(autoBox.isAutomatic());
    REQUIRE(autoBox.maintenanceDue(70000));
}

TEST_CASE("CarBrand и CarModel: модельный ряд", "[core]") {
    CarBrand bmw("BMW", "Германия", 1916);
    CarModel x5("X5", &bmw, "кроссовер", 2019, 2024);
    CarModel i3("i3", &bmw, "хэтчбек", 2013, 2020);
    bmw.addModel(x5);
    bmw.addModel(i3);

    REQUIRE(bmw.findModel("X5") == &x5);
    REQUIRE(bmw.findModel("X7") == nullptr);
    REQUIRE(bmw.filterByBodyType("кроссовер").size() == 1);
    REQUIRE(bmw.modelsReleasedIn(2019).size() == 2);
    REQUIRE(bmw.modelsReleasedIn(2015).size() == 1);
    REQUIRE(x5.matchesYear(2020));
    REQUIRE_FALSE(x5.matchesYear(2018));
    REQUIRE(x5.fullName() == "BMW X5");
}

TEST_CASE("VehicleDocument: сроки действия", "[core]") {
    VehicleDocument doc("ПТС", "AB12345678", "2019-05-01", "2029-05-01");
    Vehicle vehicle("1M8GDM9AXKP042788", 2019, "Серый");
    doc.linkVehicle(vehicle);
    REQUIRE_FALSE(doc.isExpired("2026-01-01"));
    REQUIRE(doc.isValid("2026-01-01") == false); // владелец не назначен
    Client owner("C-4", "A B", "+7", "a@b.ru");
    doc.transferTo(owner);
    REQUIRE(doc.isValid("2026-01-01"));
    REQUIRE(doc.daysUntilExpiry("2026-01-01") > 1000);

    VehicleDocument old("СТС", "XY98765432", "2015-01-01", "2020-01-01");
    REQUIRE(old.isExpired("2026-01-01"));
}

TEST_CASE("DateUtils: разбор дат", "[core]") {
    REQUIRE(date::isDate("2026-10-12"));
    REQUIRE_FALSE(date::isDate("2026-13-01"));
    REQUIRE_FALSE(date::isDate("abc"));
    REQUIRE(date::daysBetween("2026-01-01", "2026-01-11") == 10);
    REQUIRE(date::minutesOfDateTime("2026-10-12 09:30") == 570);
}
