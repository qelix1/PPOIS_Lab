#ifndef STUDENTIS_ACADEMIC_COURSEMATERIAL_H
#define STUDENTIS_ACADEMIC_COURSEMATERIAL_H

#include <string>

namespace studentis {

class Course;

/**
 * @brief Учебный материал курса: ссылка, документ, презентация или видео.
 */
class CourseMaterial {
public:
    CourseMaterial() = default;
    CourseMaterial(std::string id, Course* course, std::string title, std::string type,
                   std::string url, std::string uploadedAt, int sizeKb, bool mandatory);

    /// Материал является ссылкой.
    bool isLink() const;
    /// Материал обязателен для изучения.
    bool isMandatory() const { return mandatory_; }
    /// Тип материала совпадает с запросом.
    bool matchesType(const std::string& type) const;
    /// Переименовать материал; пустое название — ошибка данных.
    void rename(const std::string& title);
    /// Материал загружен раньше указанной даты.
    bool olderThan(const std::string& date) const;
    /// Краткое текстовое описание материала.
    std::string summary() const;

    const std::string& getId() const { return id_; }
    Course* getCourse() const { return course_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getType() const { return type_; }
    const std::string& getUrl() const { return url_; }
    const std::string& getUploadedAt() const { return uploadedAt_; }
    int getSizeKb() const { return sizeKb_; }

    void setCourse(Course* course) { course_ = course; }
    void setUrl(const std::string& url) { url_ = url; }
    void setMandatory(bool mandatory) { mandatory_ = mandatory; }

private:
    std::string id_;
    Course* course_ = nullptr;
    std::string title_;
    std::string type_;
    std::string url_;
    std::string uploadedAt_;
    int sizeKb_ = 0;
    bool mandatory_ = false;
};

} // namespace studentis

#endif
