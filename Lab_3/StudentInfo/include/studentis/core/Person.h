#ifndef STUDENTIS_CORE_PERSON_H
#define STUDENTIS_CORE_PERSON_H

#include <string>

namespace studentis {

/**
 * @brief Человек: ФИО, дата рождения, контакты и адрес.
 *
 * Базовый класс для студентов и преподавателей.
 */
class Person {
public:
    Person() = default;
    Person(std::string id, std::string lastName, std::string firstName, std::string middleName,
           std::string birthDate, std::string gender, std::string phone, std::string email,
           std::string address);

    /// Полное ФИО без лишних пробелов: «Фамилия Имя Отчество».
    std::string fullName() const;
    /// Короткое ФИО: «Фамилия И.О.».
    std::string shortName() const;
    /// Возраст в полных годах на указанную дату; неверная дата — исключение.
    int ageOn(const std::string& date) const;
    /// Достиг ли человек 18 лет на указанную дату.
    bool isAdultOn(const std::string& date) const;
    /// Обновить контакты; оба пустых поля — ошибка данных.
    void updateContacts(const std::string& phone, const std::string& email);
    /// Совпадение запроса с ФИО без учёта регистра.
    bool matchesName(const std::string& query) const;
    /// Сменить адрес; пустой адрес — ошибка данных.
    void changeAddress(const std::string& address);

    const std::string& getId() const { return id_; }
    const std::string& getLastName() const { return lastName_; }
    const std::string& getFirstName() const { return firstName_; }
    const std::string& getMiddleName() const { return middleName_; }
    const std::string& getBirthDate() const { return birthDate_; }
    const std::string& getGender() const { return gender_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getAddress() const { return address_; }

    void setLastName(const std::string& lastName) { lastName_ = lastName; }
    void setFirstName(const std::string& firstName) { firstName_ = firstName; }
    void setMiddleName(const std::string& middleName) { middleName_ = middleName; }
    void setPhone(const std::string& phone) { phone_ = phone; }
    void setEmail(const std::string& email) { email_ = email; }

private:
    std::string id_;
    std::string lastName_;
    std::string firstName_;
    std::string middleName_;
    std::string birthDate_;
    std::string gender_;
    std::string phone_;
    std::string email_;
    std::string address_;
};

} // namespace studentis

#endif
