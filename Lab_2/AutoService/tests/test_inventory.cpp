#include <catch2/catch_test_macros.hpp>

#include "autoservice/core/CarModel.h"
#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InsufficientStockException.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/InventoryAudit.h"
#include "autoservice/inventory/InventoryLine.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/inventory/PartCompatibility.h"
#include "autoservice/inventory/PurchaseOrder.h"
#include "autoservice/inventory/PurchaseOrderLine.h"
#include "autoservice/inventory/ReturnOrder.h"
#include "autoservice/inventory/StockReservation.h"
#include "autoservice/inventory/Supplier.h"
#include "autoservice/inventory/Warehouse.h"
#include "autoservice/inventory/WarehouseCell.h"
#include "autoservice/orders/RepairOrder.h"

using namespace autoservice;

TEST_CASE("Part: остатки и резерв", "[inventory]") {
    Part pads("SKU-001", "Колодки", 2000, 10, 3, "шт");
    pads.setMarkupPercent(50);

    SECTION("резерв и возврат") {
        pads.reserve(4);
        REQUIRE(pads.getStockQty() == 6);
        REQUIRE_THROWS_AS(pads.reserve(7), InsufficientStockException);
        REQUIRE_THROWS_AS(pads.reserve(0), InvalidAmountException);
        pads.release(4);
        REQUIRE(pads.getStockQty() == 10);
    }

    SECTION("поступление и минимум") {
        pads.reserve(8);
        REQUIRE(pads.isLowStock());
        pads.restock(5);
        REQUIRE(pads.getStockQty() == 7);
        REQUIRE_FALSE(pads.isLowStock());
        REQUIRE_THROWS_AS(pads.restock(-1), InvalidAmountException);
    }

    SECTION("отпускная цена") {
        REQUIRE(pads.sellPrice() == 3000);
    }
}

TEST_CASE("WarehouseCell: ячейка", "[inventory]") {
    WarehouseCell cell("A-01", "A", 10);
    Part part("SKU-2", "Фильтр", 500, 3, 1, "шт");

    cell.put(part, 6);
    REQUIRE(cell.getUsedUnits() == 6);
    REQUIRE_FALSE(cell.isFull());
    REQUIRE(cell.utilization() > 0.5);
    REQUIRE_THROWS_AS(cell.put(part, 5), WorkshopException);
    REQUIRE_THROWS_AS(cell.put(part, 0), InvalidAmountException);
    cell.take(part, 4);
    REQUIRE(cell.getUsedUnits() == 2);
    REQUIRE_THROWS_AS(cell.take(part, 5), InsufficientStockException);
}

TEST_CASE("StockReservation: бронь под заказ", "[inventory]") {
    Part part("SKU-3", "Масло", 800, 6, 2, "л");
    RepairOrder order("ORD-1", "2026-05-01", "2026-05-10");
    StockReservation reservation("RES-1", &part, 3, &order, "2026-05-05");

    SECTION("подтверждение и отмена") {
        reservation.confirm();
        REQUIRE(reservation.getStatus() == "confirmed");
        REQUIRE(part.getStockQty() == 3);
        REQUIRE_THROWS_AS(reservation.confirm(), WorkshopException);
        reservation.cancel();
        REQUIRE(reservation.getStatus() == "cancelled");
        REQUIRE(part.getStockQty() == 6);
        REQUIRE_THROWS_AS(reservation.cancel(), WorkshopException);
    }

    SECTION("срок действия") {
        REQUIRE_FALSE(reservation.isExpired("2026-05-04"));
        REQUIRE(reservation.isExpired("2026-05-06"));
        reservation.extend("2026-05-20");
        REQUIRE(reservation.getExpiresAt() == "2026-05-20");
    }
}

TEST_CASE("PurchaseOrder: заказ поставщику", "[inventory]") {
    Supplier supplier("SUP-1", "АвтоТрейд", "INN123", "+79002");
    Part part("SKU-4", "Свеча", 300, 0, 5, "шт");
    supplier.addPart(part);
    REQUIRE(supplier.getParts().size() == 1);
    REQUIRE(part.getSupplier() == &supplier);

    PurchaseOrder po("PO-1", "2026-05-01", "2026-05-10");
    po.setSupplier(supplier);
    PurchaseOrderLine line(&part, 10, 200);
    po.addLine(line);
    po.recalcTotal();
    REQUIRE(po.getTotalCostRub() == 2000);

    SECTION("проведение и приёмка") {
        po.submit();
        REQUIRE(po.getStatus() == "ordered");
        REQUIRE_THROWS_AS(po.submit(), InvalidOrderStatusException);
        po.receive();
        REQUIRE(po.getStatus() == "received");
        REQUIRE(part.getStockQty() == 10);
        REQUIRE(line.isFullyReceived());
        REQUIRE(line.remaining() == 0);
        REQUIRE_THROWS_AS(po.cancel(), InvalidOrderStatusException);
    }

    SECTION("отмена и просрочка") {
        po.cancel();
        REQUIRE(po.getStatus() == "cancelled");
        PurchaseOrder open("PO-2", "2026-05-01", "2026-05-05");
        REQUIRE(open.isLate("2026-05-09"));
        REQUIRE_FALSE(open.isLate("2026-05-03"));
    }

    SECTION("переприёмка запрещена") {
        po.submit();
        line.receive(4);
        REQUIRE_THROWS_AS(line.receive(7), WorkshopException);
        REQUIRE(line.remaining() == 6);
    }
}

TEST_CASE("Supplier: рейтинг", "[inventory]") {
    Supplier supplier("SUP-2", "Мотекс", "INN9", "+79003");
    supplier.rateDelivery(4);
    supplier.rateDelivery(5);
    REQUIRE(supplier.getRating() >= 4.0);
    REQUIRE(supplier.isPreferred(3.0));
    REQUIRE_FALSE(supplier.isPreferred(5.0));
    REQUIRE_THROWS_AS(supplier.rateDelivery(0), InvalidAmountException);
    REQUIRE_FALSE(supplier.ratingGrade().empty());

    PurchaseOrder po("PO-9", "2026-05-01", "2026-05-10");
    supplier.placeOrder(po);
    REQUIRE(po.getSupplier() == &supplier);
    REQUIRE(supplier.getOrders().size() == 1);
}

TEST_CASE("Warehouse и инвентаризация", "[inventory]") {
    Warehouse warehouse("WH-1", "Основной склад", "ул. Заводская, 1", 100);
    WarehouseCell cell("B-01", "B", 50);
    warehouse.addCell(cell);
    REQUIRE(warehouse.findCell("B-01") == &cell);
    REQUIRE(warehouse.findCell("X") == nullptr);
    REQUIRE(warehouse.freeSpace() == 50);

    Part part("SKU-5", "Амортизатор", 4000, 4, 2, "шт");
    InventoryLine matchLine(&part, 4, 4);
    InventoryLine diffLine(&part, 4, 2);
    REQUIRE(matchLine.isMatch());
    REQUIRE(diffLine.diff() == -2);
    REQUIRE_FALSE(diffLine.isMatch());
    REQUIRE(diffLine.describe().find("ожидалось 4") != std::string::npos);

    InventoryAudit audit("AUD-1", "2026-05-31");
    audit.addLine(matchLine);
    audit.addLine(diffLine);
    warehouse.startAudit(audit);
    REQUIRE(audit.getWarehouse() == &warehouse);
    audit.finish("2026-05-31");
    REQUIRE(audit.getDiscrepancies() == 1);
    REQUIRE(audit.hasIssues());
    REQUIRE(audit.totalDiscrepancy() == 2);
}

TEST_CASE("ReturnOrder: возврат поставщику", "[inventory]") {
    Supplier supplier("SUP-3", "ТормозТорг", "INN5", "+79004");
    Part part("SKU-6", "Диск тормозной", 3500, 2, 1, "шт");
    ReturnOrder ret("RET-1", date::today(), "Брак партии");

    SECTION("отмена без поставщика и запчастей") {
        REQUIRE_THROWS_AS(ret.submit(), WorkshopException);
        ret.setSupplier(supplier);
        REQUIRE_THROWS_AS(ret.submit(), WorkshopException);
    }

    SECTION("успешный возврат") {
        supplier.placeReturn(ret);
        ret.addPart(part);
        REQUIRE(ret.isReturnable(date::today()));
        ret.submit();
        REQUIRE(ret.getStatus() == "submitted");
        REQUIRE(ret.refundAmount() == 3500);
        REQUIRE_THROWS_AS(ret.submit(), InvalidOrderStatusException);
    }
}

TEST_CASE("PartCompatibility: совместимость", "[inventory]") {
    Part part("SKU-7", "Колодки", 2000, 5, 1, "шт");
    PartCompatibility universal(&part, nullptr, true);
    REQUIRE(universal.getIsUniversal());

    CarModel camry("Camry", nullptr, "седан", 2018, 2024);
    CarModel corolla("Corolla", nullptr, "седан", 2019, 2025);
    PartCompatibility specific(&part, &camry, false);
    REQUIRE(specific.fits(camry));
    REQUIRE_FALSE(specific.fits(corolla));
    REQUIRE(universal.fits(corolla));

    universal.addNote("универсал");
    REQUIRE(universal.getNotes() == "универсал");
}
