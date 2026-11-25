#include "studentis/assessment/GradeBook.h"

#include "studentis/academic/Discipline.h"
#include "studentis/assessment/ExamResult.h"
#include "studentis/assessment/Grade.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

GradeBook::GradeBook(Student* student) : student_(student) {
    if (student_ == nullptr) throw InvalidStudentDataException("зачётная книжка должна принадлежать студенту");
}

void GradeBook::addGrade(Grade& grade) {
    if (grade.getStudent() != student_) {
        throw InvalidStudentDataException("оценка принадлежит другому студенту");
    }
    for (Grade*& existing : grades_) {
        if (existing == &grade) return;
        if (existing->getDiscipline() == grade.getDiscipline() &&
            existing->getControlForm() == grade.getControlForm() &&
            existing->getSemester() == grade.getSemester()) {
            existing = &grade;
            return;
        }
    }
    grades_.push_back(&grade);
}

void GradeBook::addExamResult(ExamResult& result) {
    if (result.getStudent() != student_) {
        throw InvalidStudentDataException("результат экзамена принадлежит другому студенту");
    }
    for (ExamResult*& existing : results_) {
        if (existing == &result) return;
        if (existing->getExam() == result.getExam()) {
            existing = &result;
            return;
        }
    }
    results_.push_back(&result);
}

double GradeBook::average() const {
    if (grades_.empty()) return 0.0;
    double total = 0.0;
    for (const Grade* grade : grades_) total += static_cast<double>(grade->getValue());
    return total / static_cast<double>(grades_.size());
}

double GradeBook::averageForSemester(const Semester& semester) const {
    double total = 0.0;
    int count = 0;
    for (const Grade* grade : grades_) {
        if (grade->matchesSemester(semester)) {
            total += static_cast<double>(grade->getValue());
            ++count;
        }
    }
    return count == 0 ? 0.0 : total / static_cast<double>(count);
}

bool GradeBook::hasDebts() const {
    for (const Grade* grade : grades_) {
        if (grade->isFail()) return true;
    }
    return false;
}

std::vector<Discipline*> GradeBook::debtDisciplines() const {
    std::vector<Discipline*> result;
    for (const Grade* grade : grades_) {
        if (!grade->isFail()) continue;
        bool known = false;
        for (const Discipline* discipline : result) {
            if (discipline == grade->getDiscipline()) {
                known = true;
                break;
            }
        }
        if (!known) result.push_back(grade->getDiscipline());
    }
    return result;
}

int GradeBook::excellentCount() const {
    int count = 0;
    for (const Grade* grade : grades_) {
        if (grade->isExcellent()) ++count;
    }
    return count;
}

double GradeBook::ratingScore() const {
    double score = average() + 2.0 * static_cast<double>(excellentCount()) -
                   5.0 * static_cast<double>(debtDisciplines().size());
    if (score < 0.0) return 0.0;
    if (score > 100.0) return 100.0;
    return score;
}

} // namespace studentis
