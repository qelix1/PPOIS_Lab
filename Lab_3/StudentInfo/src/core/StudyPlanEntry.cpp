#include "studentis/core/StudyPlanEntry.h"

#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

StudyPlanEntry::StudyPlanEntry(Discipline* discipline, int semesterNumber, int credits,
                               int lectureHours, int practiceHours, int labHours,
                               std::string controlForm, bool mandatory)
    : discipline_(discipline),
      semesterNumber_(semesterNumber),
      credits_(credits),
      lectureHours_(lectureHours),
      practiceHours_(practiceHours),
      labHours_(labHours),
      controlForm_(std::move(controlForm)),
      mandatory_(mandatory) {
    if (semesterNumber_ < 1) throw InvalidStudentDataException("номер семестра должен быть положительным");
    if (credits_ < 0) throw InvalidStudentDataException("кредиты не могут быть отрицательными");
    if (lectureHours_ < 0 || practiceHours_ < 0 || labHours_ < 0) {
        throw InvalidStudentDataException("часы строки плана не могут быть отрицательными");
    }
    changeControlForm(controlForm_);
}

int StudyPlanEntry::totalHours() const {
    return lectureHours_ + practiceHours_ + labHours_;
}

bool StudyPlanEntry::isExam() const {
    return controlForm_ == "exam";
}

bool StudyPlanEntry::isCredit() const {
    return controlForm_ == "credit";
}

bool StudyPlanEntry::isElective() const {
    return !mandatory_;
}

bool StudyPlanEntry::matchesSemester(int semesterNumber) const {
    return semesterNumber_ == semesterNumber;
}

bool StudyPlanEntry::validateHours() const {
    return semesterNumber_ >= 1 && credits_ > 0 && lectureHours_ >= 0 && practiceHours_ >= 0 &&
           labHours_ >= 0 && totalHours() > 0;
}

void StudyPlanEntry::changeControlForm(const std::string& controlForm) {
    if (controlForm != "exam" && controlForm != "credit" && controlForm != "differentiated_credit") {
        throw InvalidStudentDataException("неизвестная форма контроля: " + controlForm);
    }
    controlForm_ = controlForm;
}

} // namespace studentis
