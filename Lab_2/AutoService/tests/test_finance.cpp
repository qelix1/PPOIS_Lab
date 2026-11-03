#include <catch2/catch_test_macros.hpp>

#include "autoservice/core/Client.h"
#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/PaymentFailedException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/exceptions/WarrantyExpiredException.h"
#include "autoservice/finance/CashRegister.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/finance/FinancialTransaction.h"
#include "autoservice/finance/Invoice.h"
#include "autoservice/finance/InvoiceLine.h"
#include "autoservice/finance/LoyaltyProgram.h"
#include "autoservice/finance/Payment.h"
#include "autoservice/finance/Refund.h"
#include "autoservice/finance/Warranty.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/staff/Manager.h"

using namespace autoservice;

TEST_CASE("Invoice: выставление и оплата", "[finance]") {
    Invoice invoice("INV-1", "2026-06-01", "2026-06-15");
    InvoiceLine line("Работы", 1, 10000);
    invoice.addLine(line);

    SECTION("выставление и статусы") {
        invoice.issue();
        REQUIRE(invoice.getStatus() == "issued");
        REQUIRE(invoice.getTotalRub() == 10000);
        REQUIRE_THROWS_AS(invoice.issue(), InvalidOrderStatusException);
    }

    SECTION("оплата и долг") {
        invoice.issue();
        invoice.markPaid(4000);
        REQUIRE(invoice.balance() == 6000);
        REQUIRE(invoice.getStatus() == "issued");
        invoice.markPaid(6000);
        REQUIRE(invoice.getStatus() == "paid");
        REQUIRE(invoice.balance() == 0);
        REQUIRE_THROWS_AS(invoice.markPaid(0), InvalidAmountException);
    }

    SECTION("оплата без выставления") {
        REQUIRE_THROWS_AS(invoice.markPaid(100), InvalidOrderStatusException);
    }

    SECTION("скидка и просрочка") {
        Discount discount("D20", 20, "2027-01-01", 10, 0);
        invoice.applyDiscount(discount);
        REQUIRE(invoice.getTotalRub() == 8000);
        invoice.issue();
        REQUIRE(invoice.isOverdue("2026-06-20"));
        REQUIRE_FALSE(invoice.isOverdue("2026-06-10"));
    }
}

TEST_CASE("Payment: авторизация и подтверждение", "[finance]") {
    Invoice invoice("INV-2", "2026-06-01", "2026-06-15");
    InvoiceLine line("Запчасти", 2, 3000);
    invoice.addLine(line);
    invoice.issue();
    Payment payment("PAY-1", 6000, "карта", "2026-06-05");
    payment.setInvoice(invoice);

    payment.authorize();
    REQUIRE(payment.getStatus() == "authorized");
    REQUIRE_THROWS_AS(payment.authorize(), InvalidOrderStatusException);
    payment.confirm();
    REQUIRE(payment.getStatus() == "completed");
    REQUIRE(invoice.getStatus() == "paid");
    REQUIRE(payment.receipt().find("6000") != std::string::npos);
    REQUIRE(payment.isRefundable("2026-06-20"));
    REQUIRE_FALSE(payment.isRefundable("2027-06-20"));
    REQUIRE_THROWS_AS(payment.cancel(), InvalidOrderStatusException);

    Payment another("PAY-2", 1000, "наличные", "2026-06-05");
    another.setInvoice(invoice);
    another.authorize();
    another.cancel();
    REQUIRE(another.getStatus() == "cancelled");
    REQUIRE_THROWS_AS(another.confirm(), InvalidOrderStatusException);
}

TEST_CASE("CashRegister и операции", "[finance]") {
    CashRegister cash("CR-1", "Кассир Ольга");
    cash.open("2026-06-01");
    REQUIRE(cash.isOpen());
    REQUIRE_THROWS_AS(cash.open("2026-06-01"), PaymentFailedException);
    cash.deposit(5000);
    REQUIRE(cash.getBalanceRub() == 5000);
    cash.withdraw(2000);
    REQUIRE(cash.getBalanceRub() == 3000);
    REQUIRE_THROWS_AS(cash.withdraw(10000), PaymentFailedException);
    REQUIRE_THROWS_AS(cash.withdraw(-5), InvalidAmountException);

    FinancialTransaction income("T-1", "income", 3000, "2026-06-01");
    FinancialTransaction expense("T-2", "expense", 1000, "2026-06-01");
    cash.recordTransaction(income);
    cash.recordTransaction(expense);
    REQUIRE(cash.dayTotal() == 2000);
    REQUIRE(income.isIncoming());
    REQUIRE(expense.signedAmount() == -1000);
    expense.reverse("ошибка кассира");
    REQUIRE(expense.isIncoming());
    REQUIRE(expense.getComment().find("реверс") != std::string::npos);
    REQUIRE_THROWS_AS(FinancialTransaction("T-3", "income", 0, "2026-06-01"), InvalidAmountException);

    cash.close("2026-06-01");
    REQUIRE_FALSE(cash.isOpen());
    REQUIRE_THROWS_AS(cash.deposit(100), PaymentFailedException);
}

TEST_CASE("Discount: правила применения", "[finance]") {
    Discount discount("SALE", 15, "2027-03-01", 2, 1000);

    REQUIRE(discount.isValid("2026-06-01"));
    REQUIRE_FALSE(discount.isValid("2027-04-01"));
    REQUIRE(discount.canApplyTo(1000));
    REQUIRE_FALSE(discount.canApplyTo(999));

    discount.use();
    discount.use();
    REQUIRE(discount.getUsedCount() == 2);
    REQUIRE(discount.remainingUses() == 0);
    REQUIRE_FALSE(discount.isValid("2026-06-01"));
    REQUIRE_THROWS_AS(discount.use(), WorkshopException);

    Discount blocked("B", 10, "2027-01-01", 5, 0);
    blocked.block("нарушение условий");
    REQUIRE_FALSE(blocked.isValid("2026-06-01"));
    REQUIRE(blocked.getBlocked());
}

TEST_CASE("Warranty: гарантийные случаи", "[finance]") {
    Warranty warranty("WAR-1", "2026-01-01", "2027-01-01", 30000);
    RepairOrder order("ORD-1", "2026-06-01", "2026-06-10");

    SECTION("действующая гарантия") {
        REQUIRE_FALSE(warranty.isExpired("2026-06-01"));
        REQUIRE(warranty.daysLeft("2026-06-01") > 0);
        warranty.claim(order);
        REQUIRE(warranty.getStatus() == "claimed");
        REQUIRE(warranty.getOrder() == &order);
        REQUIRE_THROWS_AS(warranty.claim(order), WorkshopException);
    }

    SECTION("истёкшая гарантия") {
        Warranty expired("WAR-2", "2024-01-01", "2025-01-01", 30000);
        REQUIRE_THROWS_AS(expired.claim(order), WarrantyExpiredException);
    }

    SECTION("продление") {
        warranty.extend(6);
        REQUIRE(warranty.getEndsAt() == "2027-06-30");
        REQUIRE_THROWS_AS(warranty.extend(0), InvalidAmountException);
    }
}

TEST_CASE("Refund: возврат денег", "[finance]") {
    Payment payment("PAY-5", 2000, "карта", "2026-06-01");
    Refund refund("REF-1", 2000, "отказ от услуги", "2026-06-02");
    refund.setPayment(payment);
    Manager manager("MG-1", "Орлова", "СПб", 10);
    CashRegister cash("CR-2", "Кассир");
    cash.open("2026-06-02");
    cash.deposit(5000);

    REQUIRE(refund.isPending());
    refund.approve(manager);
    REQUIRE_FALSE(refund.isPending());
    REQUIRE_THROWS_AS(refund.approve(manager), InvalidOrderStatusException);
    refund.execute(cash);
    REQUIRE(refund.getStatus() == "executed");
    REQUIRE(cash.getBalanceRub() == 3000);

    Refund rejected("REF-2", 1000, "нет оснований", "2026-06-02");
    rejected.reject("возврат вне гарантии");
    REQUIRE(rejected.getStatus() == "rejected");
    REQUIRE(rejected.getReason() == "возврат вне гарантии");
    REQUIRE_THROWS_AS(rejected.reject("x"), InvalidOrderStatusException);
}

TEST_CASE("LoyaltyProgram: баллы и уровни", "[finance]") {
    LoyaltyProgram program("Бонус", 2.0, 200, 100);
    program.addTier(0, "базовый");
    program.addTier(500, "серебряный");

    REQUIRE(program.accrue(1000) == 2000);
    REQUIRE_THROWS_AS(program.accrue(0), InvalidAmountException);

    Client client("C-1", "Тест", "+7", "t@t.ru");
    client.addBonus(300);
    REQUIRE(program.redeem(250, client));
    REQUIRE(client.getBonusPoints() == 50);
    REQUIRE_THROWS_AS(program.redeem(100, client), WorkshopException);
    REQUIRE_THROWS_AS(program.redeem(-1, client), InvalidAmountException);

    REQUIRE(program.tierOf(50) == "базовый");
    REQUIRE(program.tierOf(500) == "серебряный");

    program.deactivate();
    REQUIRE_FALSE(program.getActive());
    REQUIRE_THROWS_AS(program.redeem(200, client), WorkshopException);
}
