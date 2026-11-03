# README — лабораторная работа №2. Автомастерская (AutoService)

Предметная область: **автомастерская** — приёмка клиентов, диагностика,
ремонт по заказам-нарядам, склад запчастей, кадры, финансы и гарантии.

## Структура проекта

```
AutoService/
├── include/autoservice/
│   ├── core/         — автомобиль, клиент, узлы автомобиля (7 классов + DateUtils)
│   ├── exceptions/   — иерархия исключений (15 классов)
│   ├── finance/      — касса, счета, платежи, скидки, гарантии (9 классов)
│   ├── inventory/    — склад, запчасти, поставщики, инвентаризация (11 классов)
│   ├── orders/       — заказ-наряд, смета, диагностика, услуги (10 классов)
│   ├── staff/        — иерархия персонала (9 классов)
│   └── workshop/     — мастерская, посты, записи, оборудование (9 классов)
└── src/              — реализация (по одному .cpp на класс + main.cpp)
```

## Формат таблицы

`Класс | число полей | число методов (содержательных, без геттеров) | связанные классы`

## Классы

### core/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| CarBrand | 4 | 4 | CarModel |
| CarModel | 6 | 4 | CarBrand, Part, PartCompatibility |
| Client | 9 | 7 | Discount, LoyaltyProgram, Vehicle |
| Engine | 7 | 5 | Vehicle |
| Transmission | 6 | 5 | Vehicle |
| Vehicle | 10 | 12 | CarBrand, CarModel, Client, Engine, Transmission |
| VehicleDocument | 6 | 5 | Client, Vehicle |

### staff/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Employee | 10 | 7 | Workshop |
| Director | 4 | 6 | Employee (наследует), ShopReport, Workshop |
| Manager | 5 | 6 | Employee (наследует), Discount, Invoice, MarketingCampaign, Mechanic, RepairOrder |
| Mechanic | 6 | 6 | Employee (наследует), RepairOrder, Service, Skill, WorkItem |
| ServiceAdvisor | 6 | 6 | Employee (наследует), Appointment, Client, RepairOrder |
| Salary | 6 | 4 | Employee |
| Shift | 4 | 4 | Employee, ServiceBay |
| Skill | 4 | 3 | — |
| WorkSchedule | 5 | 5 | Employee, Shift |

### orders/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| RepairOrder | 12 | 14 | Client, DiagnosticReport, Estimate, Invoice, Mechanic, ServiceAdvisor, Vehicle, WorkItem |
| WorkItem | 8 | 10 | Fault, Mechanic, Service |
| Estimate | 8 | 7 | Discount, EstimateLine, Manager, RepairOrder |
| EstimateLine | 7 | 7 | Estimate, Part, Service |
| DiagnosticReport | 8 | 8 | Fault, Mechanic, Vehicle |
| RepairPlan | 7 | 6 | Manager, Part, RepairOrder, WorkItem |
| QualityCheck | 8 | 8 | Mechanic, RepairOrder |
| Fault | 7 | 6 | RepairOrder |
| Service | 7 | 5 | ServiceCategory |
| ServiceCategory | 4 | 4 | Service |

### inventory/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Part | 11 | 11 | CarModel, PartCompatibility, Supplier, WarehouseCell |
| Warehouse | 6 | 6 | Employee, InventoryAudit, PurchaseOrder, WarehouseCell |
| WarehouseCell | 6 | 4 | Part |
| Supplier | 9 | 8 | Part, PurchaseOrder, ReturnOrder |
| PurchaseOrder | 8 | 9 | Employee, PurchaseOrderLine, Supplier |
| PurchaseOrderLine | 6 | 7 | Part, PurchaseOrder |
| ReturnOrder | 7 | 6 | Part, PurchaseOrder, Supplier |
| StockReservation | 6 | 4 | Part, RepairOrder |
| InventoryAudit | 7 | 7 | Employee, InventoryLine, Warehouse |
| InventoryLine | 5 | 5 | InventoryAudit, Part |
| PartCompatibility | 4 | 3 | CarModel, Part |

### finance/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| CashRegister | 7 | 8 | FinancialTransaction, Workshop |
| FinancialTransaction | 7 | 7 | CashRegister, Invoice |
| Invoice | 10 | 9 | Client, Discount, InvoiceLine, RepairOrder |
| InvoiceLine | 7 | 7 | Invoice, Part, WorkItem |
| Payment | 8 | 8 | CashRegister, Client, Invoice |
| Refund | 8 | 6 | CashRegister, Invoice, Manager, Payment |
| Discount | 8 | 5 | — |
| LoyaltyProgram | 7 | 6 | Client, Discount |
| Warranty | 7 | 6 | Part, RepairOrder |

### workshop/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Workshop | 9 | 8 | Appointment, CashRegister, Employee, ServiceBay |
| ServiceBay | 7 | 7 | Equipment, RepairOrder, Workshop |
| Appointment | 10 | 11 | Client, Service, ServiceAdvisor, ServiceBay, Vehicle |
| SchedulePlanner | 5 | 6 | Appointment |
| Equipment | 9 | 6 | EquipmentMaintenance, ServiceBay |
| EquipmentMaintenance | 8 | 7 | Employee, Equipment |
| ClientFeedback | 8 | 6 | Client, Manager, RepairOrder |
| ShopReport | 9 | 7 | ClientFeedback, Workshop |
| MarketingCampaign | 8 | 6 | Discount |

### Исключения (15)

Базовый класс `WorkshopException` наследует `std::runtime_error`;
наследники: `AppointmentConflictException`, `ClientNotFoundException`,
`InsufficientStockException`, `InvalidAmountException`,
`InvalidClientDataException`, `InvalidOrderStatusException`,
`InvalidVinException`, `MechanicNotAvailableException`,
`PartNotFoundException`, `PaymentFailedException`,
`RepairOrderNotFoundException`, `ServiceBayBusyException`,
`VehicleNotFoundException`, `WarrantyExpiredException`.

| Исключение | Поля | Методы | Ассоциации |
|---|---|---|---|
| WorkshopException | 0 | 0 | — |
| AppointmentConflictException | 0 | 0 | WorkshopException |
| ClientNotFoundException | 0 | 0 | WorkshopException |
| InsufficientStockException | 0 | 0 | WorkshopException |
| InvalidAmountException | 0 | 0 | WorkshopException |
| InvalidClientDataException | 0 | 0 | WorkshopException |
| InvalidOrderStatusException | 0 | 0 | WorkshopException |
| InvalidVinException | 0 | 0 | WorkshopException |
| MechanicNotAvailableException | 0 | 0 | WorkshopException |
| PartNotFoundException | 0 | 0 | WorkshopException |
| PaymentFailedException | 0 | 0 | WorkshopException |
| RepairOrderNotFoundException | 0 | 0 | WorkshopException |
| ServiceBayBusyException | 0 | 0 | WorkshopException |
| VehicleNotFoundException | 0 | 0 | WorkshopException |
| WarrantyExpiredException | 0 | 0 | WorkshopException |

## Итоговая статистика

| Показатель | Значение |
|---|---|
| Классов | 70 (55 доменных + 15 исключений) |
| Поля | 391 |
| Содержательные поведения (без геттеров) | 360 |
| Ассоциации (уникальные пары класс↔класс) | ~130 |
| Исключения | 15 |

Требования задания выполнены: ≥50 классов, ≥150 полей, ≥100 поведений,
≥30 ассоциаций, ≥12 исключений.
