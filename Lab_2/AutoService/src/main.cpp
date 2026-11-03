#include <iostream>
#include <string>

#include "autoservice/core/CarBrand.h"
#include "autoservice/core/CarModel.h"
#include "autoservice/core/Client.h"
#include "autoservice/core/DateUtils.h"
#include "autoservice/core/Vehicle.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/CashRegister.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/finance/Invoice.h"
#include "autoservice/finance/InvoiceLine.h"
#include "autoservice/finance/LoyaltyProgram.h"
#include "autoservice/finance/Payment.h"
#include "autoservice/finance/Warranty.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/inventory/StockReservation.h"
#include "autoservice/inventory/WarehouseCell.h"
#include "autoservice/orders/Estimate.h"
#include "autoservice/orders/EstimateLine.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/orders/Service.h"
#include "autoservice/orders/WorkItem.h"
#include "autoservice/staff/Manager.h"
#include "autoservice/staff/Mechanic.h"
#include "autoservice/workshop/Appointment.h"
#include "autoservice/workshop/SchedulePlanner.h"

namespace {

using autoservice::Appointment;
using autoservice::CarBrand;
using autoservice::CarModel;
using autoservice::CashRegister;
using autoservice::Client;
using autoservice::Discount;
using autoservice::Estimate;
using autoservice::EstimateLine;
using autoservice::Invoice;
using autoservice::InvoiceLine;
using autoservice::LoyaltyProgram;
using autoservice::Manager;
using autoservice::Mechanic;
using autoservice::Part;
using autoservice::Payment;
using autoservice::RepairOrder;
using autoservice::SchedulePlanner;
using autoservice::Service;
using autoservice::StockReservation;
using autoservice::Vehicle;
using autoservice::Warranty;
using autoservice::WarehouseCell;
using autoservice::WorkItem;
using autoservice::WorkshopException;
using autoservice::date::today;

void printMenu() {
    std::cout << "\n=== Автомастерская: демонстрация объектной модели ===\n"
              << "1. Клиент и автомобиль (бонусы, VIN, пробег, ТО)\n"
              << "2. Заказ-наряд (работы, механик, статусы, итог)\n"
              << "3. Смета, счёт и оплата (скидка, касса)\n"
              << "4. Склад (резерв запчасти, ячейка)\n"
              << "5. Запись клиента (слоты, конфликты)\n"
              << "6. Гарантия и программа лояльности\n"
              << "0. Выход\n"
              << "> ";
}

void demoClientAndVehicle(Client& client, Vehicle& vehicle) {
    std::cout << "VIN " << vehicle.getVin() << ": ";
    vehicle.validateVin();
    std::cout << "корректен (ISO 3779)\n";
    vehicle.increaseMileage(15000);
    std::cout << "пробег теперь " << vehicle.getMileageKm() << " км\n";
    std::cout << "нужно ТО (интервал 10000): " << (vehicle.needsServiceByMileage(10000) ? "да" : "нет") << "\n";
    std::cout << "пригоден к эксплуатации: " << (vehicle.isRoadworthy(2026) ? "да" : "нет") << "\n";
    std::cout << "среднемесячный пробег за 2 года: "
              << vehicle.averageMonthlyMileage(24) << " км\n";
    client.addBonus(500);
    std::cout << "начислено 500 баллов, баланс: " << client.getBonusPoints() << "\n";
    client.spendBonus(200);
    std::cout << "списано 200 баллов, баланс: " << client.getBonusPoints() << "\n";
    std::cout << "лояльный клиент (>=100): " << (client.isLoyal(100) ? "да" : "нет") << "\n";
}

void demoOrder(RepairOrder& order, Mechanic& mechanic, WorkItem& w1, WorkItem& w2) {
    order.addWork(w1);
    order.addWork(w2);
    std::cout << "работ в заказе: " << order.getWorks().size()
              << ", итого " << order.calculateTotal() << " руб.\n";
    order.changeStatus("in_progress");
    mechanic.acceptOrder(order);
    std::cout << "статус: " << order.getStatus()
              << ", механик: " << order.getMechanic()->getFullName() << "\n";
    w1.recalcPrice(2500);
    w2.recalcPrice(2500);
    std::cout << "после пересчёта по ставке 2500 руб/ч итог: " << order.calculateTotal() << " руб.\n";
    order.close();
    std::cout << "заказ закрыт, статус: " << order.getStatus() << "\n";
}

void demoFinance(Estimate& estimate, Invoice& invoice, Payment& payment, Discount& discount,
                 Manager& manager, CashRegister& cash) {
    estimate.applyDiscount(discount);
    std::cout << "смета " << estimate.getNumber() << " со скидкой "
              << discount.getPercent() << "%: " << estimate.getTotalRub() << " руб.\n";
    estimate.approve(manager);
    std::cout << "смета утверждена, статус: " << estimate.getStatus() << "\n";
    invoice.issue();
    std::cout << "счёт выставлен: " << invoice.getTotalRub() << " руб., долг "
              << invoice.balance() << " руб.\n";
    cash.open(today());
    payment.authorize();
    payment.confirm();
    std::cout << payment.receipt() << "\n";
    std::cout << "остаток по счёту: " << invoice.balance()
              << " руб., касса: " << cash.getBalanceRub() << " руб.\n";
}

void demoWarehouse(Part& pads, WarehouseCell& cell, StockReservation& reservation) {
    std::cout << "запчасть " << pads.getSku() << " на складе: " << pads.getStockQty() << " шт.\n";
    cell.put(pads, 3);
    std::cout << "в ячейке " << cell.getCode() << ": заполненность "
              << static_cast<int>(cell.utilization() * 100) << "%\n";
    reservation.confirm();
    std::cout << "резерв под заказ подтверждён, остаток: " << pads.getStockQty() << " шт.\n";
    std::cout << "минимальный остаток достигнут: " << (pads.isLowStock() ? "да" : "нет") << "\n";
}

void demoAppointments(SchedulePlanner& planner, Appointment& a1, Appointment& a2) {
    planner.book(a1);
    std::cout << "запись " << a1.getStartsAt() << " забронирована, загрузка дня "
              << planner.dayLoad() << "%\n";
    try {
        planner.book(a2);
        std::cout << "вторая запись поставлена в очередь\n";
    } catch (const WorkshopException& e) {
        std::cout << "конфликт записи: " << e.what() << "\n";
    }
    std::cout << "ближайший свободный слот: " << planner.findFreeSlot() << "\n";
    a1.checkIn();
    a1.checkOut();
    std::cout << "запись завершена, статус: " << a1.getStatus() << "\n";
}

void demoWarrantyAndLoyalty(Warranty& warranty, LoyaltyProgram& program, Client& client) {
    std::cout << "гарантия действует ещё " << warranty.daysLeft(today()) << " дней\n";
    warranty.extend(6);
    std::cout << "гарантия продлена, новый срок: " << warranty.getEndsAt() << "\n";
    program.addTier(0, "базовый");
    program.addTier(300, "серебряный");
    program.addTier(700, "золотой");
    std::cout << "баллы клиента: " << client.getBonusPoints()
              << ", уровень: " << client.loyaltyTier(program) << "\n";
    int accrued = program.accrue(10000);
    client.addBonus(accrued);
    std::cout << "за покупку 10000 руб. начислено " << accrued << " баллов, уровень: "
              << client.loyaltyTier(program) << "\n";
}

} // namespace

int main() {
    Client client("C-001", "Иванов Иван Иванович", "+79001112233", "ivanov@mail.ru");
    Vehicle vehicle("1M8GDM9AXKP042788", 2019, "Серый");
    vehicle.assignOwner(client);

    CarBrand toyota("Toyota", "Япония", 1937);
    CarModel camry("Camry", &toyota, "седан", 2018, 2024);
    toyota.addModel(camry);
    vehicle.setBrand(toyota);
    vehicle.setModel(camry);
    vehicle.setMileage(42000);

    Mechanic mechanic("M-01", "Петров Пётр Петрович", 8, "Двигатели");
    Manager manager("MG-01", "Сидорова Анна Викторовна", "Москва", 15);

    RepairOrder order("ORD-2026-001", today(), "2026-10-20");
    order.setClient(client);
    order.setVehicle(vehicle);
    Service svcOil("S-OIL", "Замена масла", 2400, 2, 1, true);
    Service svcBrake("S-BRK", "Замена колодок", 4800, 4, 2, false);
    WorkItem w1("W-1", 3, 0);
    w1.setService(svcOil);
    WorkItem w2("W-2", 4, 0);
    w2.setService(svcBrake);

    Estimate estimate("EST-2026-001", today());
    EstimateLine l1("Замена масла", 1, 2400);
    EstimateLine l2("Колодки передние", 1, 4800);
    estimate.addLine(l1);
    estimate.addLine(l2);
    estimate.recalcTotal();

    Invoice invoice("INV-2026-001", today(), "2026-10-30");
    invoice.setClient(client);
    invoice.setOrder(order);
    InvoiceLine il1("Работы по смете EST-2026-001", 1, 6480);
    invoice.addLine(il1);
    Payment payment("PAY-2026-001", 5832, "карта", today());
    payment.setInvoice(invoice);
    CashRegister cash("CR-01", "Кассир Ольга");

    Discount discount("AUTUMN10", 10, "2026-12-31", 100, 1000);

    Part pads("SKU-001", "Колодки тормозные передние", 2400, 10, 3, "шт");
    WarehouseCell cell("A-01-02", "A", 20);
    StockReservation reservation("RES-01", &pads, 2, &order, "2026-10-25");

    SchedulePlanner planner("2026-10-15", 8, 1);
    Appointment a1("AP-01", "2026-10-15 10:00", "2026-10-15 11:00");
    a1.setClient(client);
    Appointment a2("AP-02", "2026-10-15 10:30", "2026-10-15 11:30");
    a2.setClient(client);

    Warranty warranty("WAR-01", "2025-06-01", "2027-06-30", 60000);
    LoyaltyProgram program("DriveBonus", 1.0, 100, 100);

    bool running = true;
    while (running) {
        printMenu();
        std::string choice;
        if (!std::getline(std::cin, choice)) break;
        try {
            if (choice == "1") {
                demoClientAndVehicle(client, vehicle);
            } else if (choice == "2") {
                demoOrder(order, mechanic, w1, w2);
            } else if (choice == "3") {
                demoFinance(estimate, invoice, payment, discount, manager, cash);
            } else if (choice == "4") {
                demoWarehouse(pads, cell, reservation);
            } else if (choice == "5") {
                demoAppointments(planner, a1, a2);
            } else if (choice == "6") {
                demoWarrantyAndLoyalty(warranty, program, client);
            } else if (choice == "0") {
                running = false;
            } else {
                std::cout << "неизвестный пункт меню\n";
            }
        } catch (const WorkshopException& e) {
            std::cout << "ошибка мастерской: " << e.what() << "\n";
        }
    }
    std::cout << "до свидания!\n";
    return 0;
}
