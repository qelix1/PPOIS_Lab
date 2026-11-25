#include "studentis/assessment/ExamTicket.h"

#include "studentis/assessment/Exam.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownDifficulty(const std::string& difficulty) {
    return difficulty == "easy" || difficulty == "medium" || difficulty == "hard";
}

} // namespace

ExamTicket::ExamTicket(int number, Exam* exam, std::string question1, std::string question2,
                       std::string question3, std::string difficulty)
    : number_(number),
      exam_(exam),
      question1_(std::move(question1)),
      question2_(std::move(question2)),
      question3_(std::move(question3)) {
    if (number_ <= 0) throw InvalidStudentDataException("номер билета должен быть положительным");
    if (exam_ == nullptr) throw InvalidStudentDataException("билет должен относиться к экзамену");
    if (question1_.empty()) throw InvalidStudentDataException("первый вопрос билета не может быть пустым");
    changeDifficulty(difficulty);
}

int ExamTicket::questionCount() const {
    int count = 0;
    if (!question1_.empty()) ++count;
    if (!question2_.empty()) ++count;
    if (!question3_.empty()) ++count;
    return count;
}

bool ExamTicket::isHard() const {
    return difficulty_ == "hard";
}

bool ExamTicket::matchesNumber(int number) const {
    return number_ == number;
}

void ExamTicket::changeDifficulty(const std::string& difficulty) {
    if (!isKnownDifficulty(difficulty)) {
        throw InvalidStudentDataException("неизвестная сложность билета: " + difficulty);
    }
    difficulty_ = difficulty;
}

std::string ExamTicket::summary() const {
    return "билет " + std::to_string(number_) + ": " + difficulty_ + ", вопросов " +
           std::to_string(questionCount());
}

} // namespace studentis
