#ifndef STUDENTIS_ASSESSMENT_EXAMTICKET_H
#define STUDENTIS_ASSESSMENT_EXAMTICKET_H

#include <string>

namespace studentis {

class Exam;

/**
 * @brief Экзаменационный билет: номер, три вопроса и сложность.
 *
 * Сложности: easy, medium, hard.
 */
class ExamTicket {
public:
    ExamTicket() = default;
    ExamTicket(int number, Exam* exam, std::string question1, std::string question2,
               std::string question3, std::string difficulty);

    /// Число заполненных вопросов (0..3).
    int questionCount() const;
    /// Билет повышенной сложности.
    bool isHard() const;
    /// Номер билета совпадает с запросом.
    bool matchesNumber(int number) const;
    /// Изменить сложность: easy, medium, hard; иное — InvalidStudentDataException.
    void changeDifficulty(const std::string& difficulty);
    /// Краткое текстовое описание билета.
    std::string summary() const;

    int getNumber() const { return number_; }
    Exam* getExam() const { return exam_; }
    const std::string& getQuestion1() const { return question1_; }
    const std::string& getQuestion2() const { return question2_; }
    const std::string& getQuestion3() const { return question3_; }
    const std::string& getDifficulty() const { return difficulty_; }

private:
    int number_ = 0;
    Exam* exam_ = nullptr;
    std::string question1_;
    std::string question2_;
    std::string question3_;
    std::string difficulty_ = "medium";
};

} // namespace studentis

#endif
