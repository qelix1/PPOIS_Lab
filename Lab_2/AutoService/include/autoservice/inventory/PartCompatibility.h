#ifndef AUTOSERVICE_INVENTORY_PART_COMPATIBILITY_H
#define AUTOSERVICE_INVENTORY_PART_COMPATIBILITY_H

#include <string>

namespace autoservice {

class Part;
class CarModel;

/**
 * @brief Совместимость запчасти с конкретной моделью автомобиля.
 */
class PartCompatibility {
public:
    PartCompatibility();
    PartCompatibility(Part* part, CarModel* model, bool isUniversal);

    /// Запчасть подходит под модель (или универсальна).
    bool fits(const CarModel& model) const;
    /// Добавить примечание о совместимости (ограничение, поколение).
    void addNote(const std::string& note);
    /// Сравнить записи по артикулу запчасти (<, 0, >).
    int compare(const PartCompatibility& other) const;

    Part* getPart() const { return part_; }
    CarModel* getModel() const { return model_; }
    const std::string& getNotes() const { return notes_; }
    bool getIsUniversal() const { return isUniversal_; }

private:
    Part* part_ = nullptr;
    CarModel* model_ = nullptr;
    std::string notes_;
    bool isUniversal_ = false;
};

} // namespace autoservice

#endif
