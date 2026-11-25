#include "studentis/academic/CourseMaterial.h"

#include "studentis/core/DateUtils.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

CourseMaterial::CourseMaterial(std::string id, Course* course, std::string title, std::string type,
                               std::string url, std::string uploadedAt, int sizeKb, bool mandatory)
    : id_(std::move(id)),
      course_(course),
      title_(std::move(title)),
      type_(std::move(type)),
      url_(std::move(url)),
      uploadedAt_(std::move(uploadedAt)),
      sizeKb_(sizeKb),
      mandatory_(mandatory) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор материала не может быть пустым");
    if (title_.empty()) throw InvalidStudentDataException("название материала не может быть пустым");
    if (!date::isDate(uploadedAt_)) {
        throw InvalidStudentDataException("некорректная дата загрузки: " + uploadedAt_);
    }
    if (sizeKb_ < 0) throw InvalidStudentDataException("размер материала не может быть отрицательным");
}

bool CourseMaterial::isLink() const {
    return type_ == "link";
}

bool CourseMaterial::matchesType(const std::string& type) const {
    return type_ == type;
}

void CourseMaterial::rename(const std::string& title) {
    if (title.empty()) throw InvalidStudentDataException("название материала не может быть пустым");
    title_ = title;
}

bool CourseMaterial::olderThan(const std::string& date) const {
    if (!date::isDate(date)) return false;
    return uploadedAt_ < date;
}

std::string CourseMaterial::summary() const {
    return title_ + " (" + type_ + ", " + std::to_string(sizeKb_) + " КБ)";
}

} // namespace studentis
