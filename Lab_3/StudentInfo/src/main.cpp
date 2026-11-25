#include <iostream>
#include <string>
#include <vector>

#include "studentis/academic/Classroom.h"
#include "studentis/academic/Course.h"
#include "studentis/academic/Discipline.h"
#include "studentis/academic/Lesson.h"
#include "studentis/academic/Schedule.h"
#include "studentis/assessment/Attendance.h"
#include "studentis/assessment/AttendanceJournal.h"
#include "studentis/assessment/Grade.h"
#include "studentis/assessment/GradeBook.h"
#include "studentis/core/AcademicYear.h"
#include "studentis/core/Curriculum.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Faculty.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Specialty.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/StudyPlanEntry.h"
#include "studentis/core/Teacher.h"
#include "studentis/core/University.h"
#include "studentis/documents/StudentCard.h"
#include "studentis/documents/Thesis.h"
#include "studentis/exceptions/DormitoryFullException.h"
#include "studentis/exceptions/ScheduleConflictException.h"
#include "studentis/exceptions/StudentInfoException.h"
#include "studentis/finance/FinanceAccount.h"
#include "studentis/finance/Scholarship.h"
#include "studentis/finance/TuitionContract.h"
#include "studentis/finance/TuitionPayment.h"
#include "studentis/services/Book.h"
#include "studentis/services/BookLoan.h"
#include "studentis/services/DormRoom.h"
#include "studentis/services/Dormitory.h"
#include "studentis/services/LibraryCard.h"
#include "studentis/services/ProgressReport.h"

using namespace studentis;

namespace {

Student makeStudent(const std::string& id, const std::string& lastName, const std::string& gender,
                    const std::string& funding = "budget") {
    return Student("P-" + id, lastName, "Иван", "Иванович", "2005-09-01", gender,
                   "+79000000000", "student@example.ru", "г. Москва", id, "2024-09-01",
                   "active", "full_time", funding, 1);
}

Teacher makeTeacher() {
    return Teacher("P-T-1", "Петров", "Пётр", "Петрович", "1970-01-01", "male",
                   "+79000000001", "teacher@example.ru", "г. Москва", "T-1", "доцент",
                   "к.т.н.", "", "2010-09-01", 700);
}

void showMenu() {
    std::cout << "\n=== Информационная система студентов ===\n"
              << "1. Зачисление и перевод между группами\n"
              << "2. Учебный план и конфликт расписания\n"
              << "3. Оценки, посещаемость и отчёт\n"
              << "4. Оплата обучения и стипендия\n"
              << "5. Студенческий пропуск и выпускная работа\n"
              << "6. Библиотека и общежитие\n"
              << "0. Выход\n"
              << "Выбор: ";
}

void enrollmentDemo() {
    University university("МГТУ им. Баумана", nullptr, 1830);
    Faculty faculty("F-1", "Факультет информатики", "ФИ", nullptr, "+7000", "корпус 1");
    university.addFaculty(faculty);
    Specialty specialty("09.03.01", "Информатика и вычислительная техника", &faculty,
                        "бакалавр", 4, 400000.0, 25);
    StudentGroup first("ИУ7-11Б", &specialty, 2024, 1, nullptr, 2, "active");
    StudentGroup second("ИУ7-12Б", &specialty, 2024, 1, nullptr, 2, "active");
    Student student = makeStudent("S-1", "Иванов", "male");

    first.addStudent(student);
    university.enrollStudent(student);
    std::cout << "Зачислен: " << student.fullName() << ", группа " << first.getCode() << "\n";
    student.transferTo(&second);
    std::cout << "После перевода: " << student.getGroup()->getCode()
              << ", студентов в университете: " << university.totalStudents() << "\n";
}

void scheduleDemo() {
    Faculty faculty("F-1", "Факультет информатики", "ФИ", nullptr, "+7000", "корпус 1");
    Specialty specialty("09.03.01", "Информатика", &faculty, "бакалавр", 4, 400000.0, 25);
    StudentGroup group("ИУ7-11Б", &specialty, 2024, 1, nullptr, 25, "active");
    Teacher teacher = makeTeacher();
    Discipline discipline("D-1", "Алгоритмы", "ALG", nullptr, 4, "Базовый курс");
    Semester semester(1, nullptr, "2025-09-01", "2025-12-31", "2026-01-05", "2026-01-25",
                      "active");
    Classroom room("R-1", "корпус 1", "501", 30, "lecture", true, false);
    Course course("C-1", &discipline, &teacher, &semester, &group, 64, &room, "active");

    Curriculum plan("CUR-1", &specialty, 2024, "2024-06-01");
    StudyPlanEntry firstEntry(&discipline, 1, 4, 32, 16, 16, "exam", true);
    plan.addEntry(firstEntry);
    std::cout << "Учебный план: " << plan.recalcCredits() << " з.е.\n";

    Schedule schedule("SCH-1", &group, &semester);
    Lesson first("L-1", &course, "2026-09-01", "10:00", "11:30", "lecture", &room, "Введение");
    Lesson second("L-2", &course, "2026-09-01", "11:00", "12:00", "practice", &room,
                  "Практика");
    schedule.addLesson(first);
    try {
        schedule.addLesson(second);
    } catch (const ScheduleConflictException& error) {
        std::cout << "Конфликт корректно отклонён: " << error.what() << "\n";
    }
    std::cout << "Занятий в расписании: " << schedule.getLessons().size() << "\n";
}

void progressDemo() {
    Teacher teacher = makeTeacher();
    Discipline algorithms("D-1", "Алгоритмы", "ALG", nullptr, 4, "");
    Discipline physics("D-2", "Физика", "PHYS", nullptr, 4, "");
    Student mainStudent = makeStudent("S-1", "Иванов", "male");
    Student second = makeStudent("S-2", "Петров", "male");
    Student third = makeStudent("S-3", "Сидоров", "male");
    Student fourth = makeStudent("S-4", "Смирнов", "male");
    StudentGroup group("ИУ7-11Б", nullptr, 2024, 1, nullptr, 25, "active");
    for (Student* student : {&mainStudent, &second, &third, &fourth}) group.addStudent(*student);
    Semester semester(1, nullptr, "2025-09-01", "2025-12-31", "2026-01-05", "2026-01-25",
                      "active");
    Course course("C-1", &algorithms, &teacher, &semester, &group, 64, nullptr, "active");
    Lesson lesson("L-1", &course, "2026-10-01", "10:00", "11:30", "lecture", nullptr, "Оценки");

    Grade first("G-1", &mainStudent, &algorithms, &teacher, 80, "2026-10-05", "exam", &semester);
    Grade secondGrade("G-2", &mainStudent, &physics, &teacher, 90, "2026-10-06", "exam",
                      &semester);
    GradeBook book(&mainStudent);
    book.addGrade(first);
    book.addGrade(secondGrade);

    Attendance present("A-1", &lesson, &mainStudent, "present", 0, "", &teacher);
    Attendance late("A-2", &lesson, &second, "late", 10, "", &teacher);
    Attendance excused("A-3", &lesson, &third, "excused", 0, "болезнь", &teacher);
    Attendance absent("A-4", &lesson, &fourth, "absent", 0, "", &teacher);
    AttendanceJournal journal(&group, &course);
    journal.mark(present);
    journal.mark(late);
    journal.mark(excused);
    journal.mark(absent);

    ProgressReport report("R-1", &mainStudent, &semester);
    report.buildFrom(book, journal, "2026-10-10");
    std::cout << "Средний балл: " << report.getAverageScore()
              << ", посещаемость: " << report.getAttendancePercent() << "%\n"
              << "Долгов: " << report.getDebtsCount()
              << ", успевает: " << (report.isSuccessful() ? "да" : "нет") << "\n"
              << "Рекомендации: " << report.getRecommendations() << "\n";
}

void financeDemo() {
    Teacher teacher = makeTeacher();
    Student student = makeStudent("S-1", "Иванов", "male", "paid");
    Faculty faculty("F-1", "Факультет информатики", "ФИ", nullptr, "+7000", "корпус 1");
    Specialty specialty("09.03.01", "Информатика", &faculty, "бакалавр", 4, 400000.0, 25);
    TuitionContract contract("TC-1", &student, &specialty, "2025-08-20", "2025-09-01",
                             "2029-06-30", 100000.0, 10.0, "active");
    std::cout << "Год с учётом скидки: " << contract.yearlyTotal()
              << ", семестр: " << contract.semesterAmount() << "\n";

    TuitionPayment payment("TP-1", &contract, contract.semesterAmount(), "2025-09-10", "card",
                           "RCPT-1");
    payment.confirm(teacher);
    FinanceAccount account(&student);
    account.charge(90000.0);
    account.registerPayment(payment);
    std::cout << "Оплачено: " << payment.getAmount() << ", долг: " << account.debt() << "\n";

    Discipline discipline("D-1", "Алгоритмы", "ALG", nullptr, 4, "");
    StudentGroup group("ИУ7-11Б", nullptr, 2024, 1, nullptr, 25, "active");
    group.addStudent(student);
    Course course("C-1", &discipline, &teacher, nullptr, &group, 64, nullptr, "active");
    GradeBook book(&student);
    Grade grade("G-1", &student, &discipline, &teacher, 92, "2025-12-20", "exam");
    book.addGrade(grade);
    AttendanceJournal journal(&group, &course);
    Scholarship scholarship("SCH-1", "Академическая", "academic", 5000.0, 80.0, 0, 10, true);
    std::cout << "Стипендия доступна: "
              << (scholarship.isEligible(student, book, journal) ? "да" : "нет") << "\n";
}

void documentsDemo() {
    Student student = makeStudent("S-1", "Иванов", "male");
    Teacher teacher = makeTeacher();
    StudentCard card("CARD-1", &student, "2025-09-01", "2030-09-01", "active", "1234", "full");
    std::cout << "Пропуск действителен: " << (card.isValid(date::today()) ? "да" : "нет")
              << ", дней до истечения: " << card.daysUntilExpiry(date::today()) << "\n";
    std::cout << "PIN верен: " << (card.verifyPin("1234") ? "да" : "нет") << "\n";

    Thesis thesis("TH-1", &student, &teacher, "Интеллектуальная система студентов",
                  "2026-05-15", "2026-06-15", 15.0, 0, "submitted");
    std::cout << "Работа допущена к защите: "
              << (thesis.isAllowedToDefend() ? "да" : "нет") << "\n";
    thesis.grade(90);
    std::cout << "Защита успешна: " << (thesis.isPassed() ? "да" : "нет")
              << ", оценка: " << thesis.getScore() << "\n";
}

void servicesDemo() {
    Teacher teacher = makeTeacher();
    Student student = makeStudent("S-1", "Иванов", "male");
    Book book("B-1", "Алгоритмы и структуры данных", "Кормен", "978-5-0000-0000-1", 2022, 2, 2);
    book.borrow();
    BookLoan loan("BL-1", &book, &student, "2025-12-20", "2026-01-10", 10.0);
    LibraryCard card("LIB-1", &student, "2025-09-01", "2030-09-01");
    card.addLoan(loan);
    std::cout << "Штраф на 2026-01-15: " << loan.calculateFine("2026-01-15")
              << ", просрочка: " << loan.overdueDays("2026-01-15") << " дн.\n";
    loan.returnBook("2026-01-15");
    std::cout << "Книга возвращена, доступных экземпляров: " << book.getCopiesAvailable()
              << "\n";

    Student second = makeStudent("S-2", "Петров", "male");
    Student third = makeStudent("S-3", "Сидоров", "male");
    Dormitory dormitory("D-1", "ул. Госпитальная, 4", 1, &teacher, 6000.0, "+7002");
    DormRoom room("401", &dormitory, 2, 4, "male");
    dormitory.addRoom(room);
    dormitory.assignStudent(student);
    std::cout << "Заполненность после одного: " << dormitory.occupancyRate() << "%\n";
    dormitory.assignStudent(second);
    try {
        dormitory.assignStudent(third);
    } catch (const DormitoryFullException& error) {
        std::cout << "Третье заселение отклонено: " << error.what() << "\n";
    }
}

} // namespace

int main() {
    std::cout << "Демонстрация объектной модели «Информационная система студентов».\n";
    while (true) {
        showMenu();
        int choice = -1;
        if (!(std::cin >> choice)) break;
        if (choice == 0) break;
        try {
            switch (choice) {
            case 1: enrollmentDemo(); break;
            case 2: scheduleDemo(); break;
            case 3: progressDemo(); break;
            case 4: financeDemo(); break;
            case 5: documentsDemo(); break;
            case 6: servicesDemo(); break;
            default: std::cout << "Неизвестный пункт меню.\n"; break;
            }
        } catch (const StudentInfoException& error) {
            std::cout << "Ошибка предметной области: " << error.what() << "\n";
        }
    }
    std::cout << "Завершение работы.\n";
    return 0;
}
