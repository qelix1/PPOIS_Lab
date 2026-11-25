#ifndef STUDENTIS_SERVICES_BOOK_H
#define STUDENTIS_SERVICES_BOOK_H

#include <string>

namespace studentis {

/**
 * @brief Книга библиотеки: выходные данные, число экземпляров и категория.
 */
class Book {
public:
    Book() = default;
    Book(std::string id, std::string title, std::string author, std::string isbn, int year,
         int copiesTotal, int copiesAvailable, std::string category = "general");

    /// Есть ли хотя бы один свободный экземпляр.
    bool isAvailable() const;
    /// Выдать экземпляр; false — свободных экземпляров нет.
    bool borrow();
    /// Вернуть экземпляр; false — все экземпляры уже на месте.
    bool returnCopy();
    /// Автор совпадает с запросом без учёта регистра (подстрока).
    bool matchesAuthor(const std::string& query) const;
    /// Название совпадает с запросом без учёта регистра (подстрока).
    bool matchesTitle(const std::string& query) const;
    /// Книга не старше пяти лет относительно указанного года.
    bool isNew(int referenceYear) const;
    /// Зарезервировать экземпляр; свободных нет — InvalidStudentDataException.
    void reserve();

    const std::string& getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }
    const std::string& getIsbn() const { return isbn_; }
    int getYear() const { return year_; }
    int getCopiesTotal() const { return copiesTotal_; }
    int getCopiesAvailable() const { return copiesAvailable_; }
    const std::string& getCategory() const { return category_; }

    void setTitle(const std::string& title) { title_ = title; }
    void setAuthor(const std::string& author) { author_ = author; }
    void setCategory(const std::string& category) { category_ = category; }

private:
    std::string id_;
    std::string title_;
    std::string author_;
    std::string isbn_;
    int year_ = 0;
    int copiesTotal_ = 0;
    int copiesAvailable_ = 0;
    std::string category_;
};

} // namespace studentis

#endif
