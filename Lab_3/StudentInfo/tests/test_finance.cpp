#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "studentis/academic/Course.h"
#include "studentis/academic/Discipline.h"
#include "studentis/academic/Lesson.h"
#include "studentis/assessment/Attendance.h"
#include "studentis/assessment/AttendanceJournal.h"
#include "studentis/assessment/Grade.h"
#include "studentis/assessment/GradeBook.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Specialty.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InsufficientFundsException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/finance/FinanceAccount.h"
#include "studentis/finance/Scholarship.h"
#include "studentis/finance/ScholarshipAward.h"
#include "studentis/finance/StudentFine.h"
#include "studentis/finance/TuitionContract.h"
#include "studentis/finance/TuitionPayment.h"

using namespace studentis;
using Catch::Approx;

namespace {

Student makeStudent(const std::string& studentId, const std::string& status = "active") {
    return Student("P-" + studentId, "Иванов", "Иван", "Иванович", "2005-09-01", "male",
                   "+79000000000", "student@example.ru", "г. Москва", studentId, "2024-09-01",
                   status, "full_time", "contract", 1);
}

Teacher makeTeacher(const std::string& teacherId = "T-1") {
    return Teacher("P-" + teacherId, "Петров", "Пётр", "Петрович", "1970-01-01", "male",
                   "+79000000001", "teacher@example.ru", "г. Москва", teacherId, "доцент",
                   "к.т.н.", "", "2010-09-01", 700);
}

Specialty makeSpecialty() {
    return Specialty("09.03.01", "Программная инженерия", nullptr, "бакалавр", 4, 400000.0, 25);
}

Semester makeSemester(int number = 1) {
    return Semester(number, nullptr, "2025-09-01", "2025-12-31", "2026-01-05", "2026-01-25",
                    "active");
}

TuitionContract makeContract(Student* student, Specialty* specialty) {
    return TuitionContract("TC-1", student, specialty, "2025-08-20", "2025-09-01", "2026-06-30",
                           100000.0, 10.0, "active");
}

} // namespace

TEST_CASE("TuitionContract: стоимость, скидка и срок действия", "[finance]") {
    Student student = makeStudent("S-1");
    Specialty specialty = makeSpecialty();

    SECTION("годовая и семестровая стоимость со скидкой") {
        TuitionContract contract = makeContract(&student, &specialty);
        REQUIRE(contract.getStudent() == &student);
        REQUIRE(contract.getSpecialty() == &specialty);
        REQUIRE(contract.yearlyTotal() == Approx(90000.0));
        REQUIRE(contract.semesterAmount() == Approx(45000.0));
        REQUIRE(contract.isActive());
        REQUIRE(contract.validateDates());
    }

    SECTION("применение скидки и расторжение") {
        TuitionContract contract = makeContract(&student, &specialty);
        contract.applyDiscount(50.0);
        REQUIRE(contract.getDiscountPercent() == Approx(50.0));
        REQUIRE(contract.yearlyTotal() == Approx(50000.0));
        REQUIRE(contract.semesterAmount() == Approx(25000.0));
        REQUIRE_THROWS_AS(contract.applyDiscount(-1.0), InvalidStudentDataException);
        REQUIRE_THROWS_AS(contract.applyDiscount(101.0), InvalidStudentDataException);

        contract.terminate();
        REQUIRE_FALSE(contract.isActive());
        REQUIRE(contract.getStatus() == "terminated");
        contract.applyDiscount(20.0);
        REQUIRE(contract.getDiscountPercent() == Approx(20.0));
        REQUIRE(contract.remainingAmount("2025-10-01") == Approx(0.0));
    }

    SECTION("даты, остаток и дни до окончания") {
        TuitionContract contract("TC-2", &student, &specialty, "2025-12-25", "2026-01-01",
                                 "2026-01-11", 100000.0, 10.0, "active");
        REQUIRE(contract.daysUntilExpiry("2026-01-01") == 10);
        REQUIRE(contract.remainingAmount("2025-12-20") == Approx(90000.0));
        REQUIRE(contract.remainingAmount("2026-01-06") == Approx(45000.0));
        REQUIRE(contract.remainingAmount("2026-01-11") == Approx(0.0));
        REQUIRE(contract.remainingAmount("2026-01-12") == Approx(0.0));
        REQUIRE_THROWS_AS(contract.daysUntilExpiry("завтра"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(contract.remainingAmount("2026-13-01"), InvalidStudentDataException);
    }

    SECTION("изменение полей с проверкой") {
        TuitionContract contract = makeContract(&student, &specialty);
        contract.setSignedAt("2025-08-21");
        contract.setStartDate("2025-09-02");
        contract.setEndDate("2026-06-29");
        contract.setAnnualAmount(120000.0);
        contract.setStatus("draft");
        REQUIRE(contract.getSignedAt() == "2025-08-21");
        REQUIRE(contract.getStartDate() == "2025-09-02");
        REQUIRE(contract.getEndDate() == "2026-06-29");
        REQUIRE(contract.getAnnualAmount() == Approx(120000.0));
        REQUIRE(contract.getStatus() == "draft");
        REQUIRE_THROWS_AS(contract.setSignedAt("вчера"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(contract.setStartDate("2026-07-01"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(contract.setEndDate("2025-01-01"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(contract.setAnnualAmount(0.0), InsufficientFundsException);
        REQUIRE_THROWS_AS(contract.setStatus("неизвестный"), InvalidStudentDataException);
    }

    SECTION("некорректные данные договора") {
        REQUIRE_THROWS_AS(TuitionContract("", &student, &specialty, "2025-08-20", "2025-09-01",
                                          "2026-06-30", 100000.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionContract("TC-3", nullptr, &specialty, "2025-08-20", "2025-09-01",
                                          "2026-06-30", 100000.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionContract("TC-3", &student, nullptr, "2025-08-20", "2025-09-01",
                                          "2026-06-30", 100000.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionContract("TC-3", &student, &specialty, "2025-08-20", "2026-06-30",
                                          "2025-09-01", 100000.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionContract("TC-3", &student, &specialty, "2025-08-20", "2025-09-01",
                                          "2026-06-30", 0.0),
                          InsufficientFundsException);
        REQUIRE_THROWS_AS(TuitionContract("TC-3", &student, &specialty, "2025-08-20", "2025-09-01",
                                          "2026-06-30", 100000.0, 101.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionContract("TC-3", &student, &specialty, "2025-08-20", "2025-09-01",
                                          "2026-06-30", 100000.0, 0.0, "неизвестный"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("TuitionPayment: подтверждение, отмена и квитанция", "[finance]") {
    Student student = makeStudent("S-1");
    Specialty specialty = makeSpecialty();
    TuitionContract contract = makeContract(&student, &specialty);
    Teacher teacher = makeTeacher();

    SECTION("подтверждение и квитанция") {
        TuitionPayment payment("TP-1", &contract, 20000.0, "2025-09-10", "card", "R-1");
        REQUIRE(payment.getContract() == &contract);
        REQUIRE(payment.getAmount() == Approx(20000.0));
        REQUIRE_FALSE(payment.isConfirmed());
        REQUIRE(payment.isLate());
        REQUIRE(payment.receipt().find("R-1") != std::string::npos);
        REQUIRE(payment.receipt().find("20000.00") != std::string::npos);
        REQUIRE(payment.summary().find("TP-1") != std::string::npos);

        payment.confirm(teacher);
        REQUIRE(payment.isConfirmed());
        REQUIRE(payment.getStatus() == "confirmed");
        REQUIRE(payment.getConfirmedBy() == &teacher);
        REQUIRE_THROWS_AS(payment.confirm(teacher), InvalidStudentDataException);
        REQUIRE_THROWS_AS(payment.cancel(), InvalidStudentDataException);

        payment.setMethod("cash");
        payment.setReceiptNumber("R-9");
        REQUIRE(payment.getMethod() == "cash");
        REQUIRE(payment.getReceiptNumber() == "R-9");
        REQUIRE_THROWS_AS(payment.setMethod(""), InvalidStudentDataException);
        REQUIRE_THROWS_AS(payment.setReceiptNumber(""), InvalidStudentDataException);
    }

    SECTION("отмена неподтверждённого платежа") {
        TuitionPayment payment("TP-2", &contract, 15000.0, "2025-08-15", "bank_transfer", "R-2");
        REQUIRE_FALSE(payment.isLate());
        payment.cancel();
        REQUIRE(payment.getStatus() == "cancelled");
        REQUIRE_THROWS_AS(payment.confirm(teacher), InvalidStudentDataException);
    }

    SECTION("период платежа") {
        TuitionPayment payment("TP-3", &contract, 10000.0, "2025-09-15", "cash", "R-3");
        REQUIRE(payment.matchesPeriod("2025-09-01", "2025-09-30"));
        REQUIRE_FALSE(payment.matchesPeriod("2025-10-01", "2025-10-31"));
        REQUIRE(payment.matchesPeriod("2025-09-15", "2025-09-15"));
        REQUIRE_THROWS_AS(payment.matchesPeriod("2025-10-01", "2025-09-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(payment.matchesPeriod("вчера", "2025-09-30"),
                          InvalidStudentDataException);
    }

    SECTION("некорректные данные платежа") {
        REQUIRE_THROWS_AS(TuitionPayment("", &contract, 1000.0, "2025-09-10", "card", "R-1"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionPayment("TP-4", nullptr, 1000.0, "2025-09-10", "card", "R-1"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionPayment("TP-4", &contract, 0.0, "2025-09-10", "card", "R-1"),
                          InsufficientFundsException);
        REQUIRE_THROWS_AS(TuitionPayment("TP-4", &contract, 1000.0, "10.09.2025", "card", "R-1"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionPayment("TP-4", &contract, 1000.0, "2025-09-10", "", "R-1"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionPayment("TP-4", &contract, 1000.0, "2025-09-10", "card", ""),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TuitionPayment("TP-4", &contract, 1000.0, "2025-09-10", "card", "R-1",
                                         nullptr, "неизвестный"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("StudentFine: частичная оплата, переплата и списание", "[finance]") {
    Student student = makeStudent("S-1");

    SECTION("частичные выплаты и погашение") {
        StudentFine fine("F-1", &student, "порча имущества", 1000.0, "2026-01-01", "2026-02-01");
        REQUIRE(fine.remaining() == Approx(1000.0));
        REQUIRE_FALSE(fine.isPaid());
        REQUIRE_FALSE(fine.isOverdue("2026-02-01"));
        REQUIRE(fine.isOverdue("2026-02-02"));

        fine.addPayment(400.0);
        REQUIRE(fine.getPaidAmount() == Approx(400.0));
        REQUIRE(fine.remaining() == Approx(600.0));
        REQUIRE_FALSE(fine.isOverdue("2026-01-15"));

        fine.addPayment(600.0);
        REQUIRE(fine.isPaid());
        REQUIRE(fine.getStatus() == "paid");
        REQUIRE(fine.remaining() == Approx(0.0));
        REQUIRE_FALSE(fine.isOverdue("2026-03-01"));
        REQUIRE_THROWS_AS(fine.addPayment(1.0), InsufficientFundsException);
        REQUIRE_THROWS_AS(fine.waive(), InvalidStudentDataException);
    }

    SECTION("переплата и неположительные суммы") {
        StudentFine fine("F-2", &student, "опоздание", 500.0, "2026-01-01", "2026-01-10");
        REQUIRE_THROWS_AS(fine.addPayment(501.0), InsufficientFundsException);
        REQUIRE(fine.remaining() == Approx(500.0));
        REQUIRE_THROWS_AS(fine.addPayment(0.0), InsufficientFundsException);
        REQUIRE_THROWS_AS(fine.addPayment(-10.0), InsufficientFundsException);
        fine.addPayment(250.0);
        REQUIRE(fine.remaining() == Approx(250.0));
    }

    SECTION("списание штрафа") {
        StudentFine fine("F-3", &student, "шум", 300.0, "2026-01-01", "2026-01-05");
        fine.waive();
        REQUIRE(fine.getStatus() == "waived");
        REQUIRE_FALSE(fine.isPaid());
        REQUIRE(fine.remaining() == Approx(300.0));
        REQUIRE_FALSE(fine.isOverdue("2026-06-01"));
        REQUIRE_THROWS_AS(fine.addPayment(100.0), InvalidStudentDataException);
    }

    SECTION("граница срока оплаты") {
        StudentFine fine("F-4", &student, "шум", 300.0, "2026-02-01", "2026-02-10");
        REQUIRE_FALSE(fine.isOverdue("2026-02-09"));
        REQUIRE_FALSE(fine.isOverdue("2026-02-10"));
        REQUIRE(fine.isOverdue("2026-02-11"));
        REQUIRE_THROWS_AS(fine.isOverdue("завтра"), InvalidStudentDataException);
    }

    SECTION("некорректные данные штрафа") {
        REQUIRE_THROWS_AS(StudentFine("", &student, "шум", 100.0, "2026-01-01", "2026-01-10"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(StudentFine("F-5", nullptr, "шум", 100.0, "2026-01-01", "2026-01-10"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(StudentFine("F-5", &student, "", 100.0, "2026-01-01", "2026-01-10"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(StudentFine("F-5", &student, "шум", 0.0, "2026-01-01", "2026-01-10"),
                          InsufficientFundsException);
        REQUIRE_THROWS_AS(StudentFine("F-5", &student, "шум", 100.0, "2026-01-01", "2025-12-31"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(StudentFine("F-5", &student, "шум", 100.0, "2026-01-01", "2026-01-10",
                                      101.0),
                          InsufficientFundsException);
        REQUIRE_THROWS_AS(StudentFine("F-5", &student, "шум", 100.0, "2026-01-01", "2026-01-10",
                                      10.0, "paid"),
                          InvalidStudentDataException);
    }

    SECTION("полностью оплаченный штраф при создании") {
        StudentFine fine("F-6", &student, "шум", 100.0, "2026-01-01", "2026-01-10", 100.0);
        REQUIRE(fine.isPaid());
        REQUIRE(fine.getStatus() == "paid");
    }
}

TEST_CASE("Scholarship: академическая и социальная стипендия", "[finance]") {
    Student student = makeStudent("S-1");
    Teacher teacher = makeTeacher();
    Discipline discipline("D-1", "Математика", "MATH", nullptr, 4, "");
    Discipline second("D-2", "Физика", "PHYS", nullptr, 4, "");

    StudentGroup group("ИУ7-11Б", nullptr, 2024, 1, nullptr, 25, "active");
    group.addStudent(student);
    Course course("C-1", &discipline, &teacher, nullptr, &group, 72, nullptr, "active");
    AttendanceJournal journal(&group, &course);
    Lesson lesson("L-1", &course, "2026-02-01", "10:00", "11:30", "lecture", nullptr, "Тема");

    Grade excellent("G-1", &student, &discipline, &teacher, 90, "2026-01-10", "exam");
    GradeBook book(&student);
    book.addGrade(excellent);

    SECTION("академическая стипендия") {
        Scholarship academic("SCH-1", "Академическая", "academic", 5000.0, 80.0, 2, 10, true);
        REQUIRE(academic.isAcademic());
        REQUIRE_FALSE(academic.isSocial());
        REQUIRE(academic.amountFor() == Approx(5000.0));
        REQUIRE(academic.requireAverage(80.0));
        REQUIRE_FALSE(academic.requireAverage(79.9));
        REQUIRE(academic.isEligible(student, book, journal));

        Attendance absent("A-1", &lesson, &student, "absent");
        journal.mark(absent);
        REQUIRE(academic.isEligible(student, book, journal));

        Scholarship strict("SCH-2", "Академическая", "academic", 5000.0, 80.0, 0, 10, true);
        REQUIRE_FALSE(strict.isEligible(student, book, journal));
        AttendanceJournal excusedJournal(&group, &course);
        Attendance excused("A-2", &lesson, &student, "excused");
        excusedJournal.mark(excused);
        REQUIRE(strict.isEligible(student, book, excusedJournal));
    }

    SECTION("низкий средний балл и задолженность") {
        Grade low("G-2", &student, &second, &teacher, 50, "2026-01-11", "exam");
        GradeBook debtBook(&student);
        debtBook.addGrade(excellent);
        debtBook.addGrade(low);
        Scholarship academic("SCH-3", "Академическая", "academic", 5000.0, 60.0, 5, 10, true);
        REQUIRE(debtBook.average() == Approx(70.0));
        REQUIRE(debtBook.hasDebts());
        REQUIRE_FALSE(academic.isEligible(student, debtBook, journal));

        GradeBook emptyBook(&student);
        Scholarship high("SCH-4", "Академическая", "academic", 5000.0, 80.0, 5, 10, true);
        REQUIRE_FALSE(high.isEligible(student, emptyBook, journal));
    }

    SECTION("социальная стипендия не зависит от среднего балла") {
        Scholarship social("SCH-5", "Социальная", "social", 3000.0, 95.0, 2, 10, true);
        REQUIRE(social.isSocial());
        REQUIRE_FALSE(social.isAcademic());
        GradeBook emptyBook(&student);
        REQUIRE(social.isEligible(student, emptyBook, journal));

        Attendance absent("A-3", &lesson, &student, "absent");
        journal.mark(absent);
        Scholarship strict("SCH-6", "Социальная", "social", 3000.0, 95.0, 0, 10, true);
        REQUIRE_FALSE(strict.isEligible(student, emptyBook, journal));
    }

    SECTION("неактивные стипендия и студент") {
        Scholarship academic("SCH-7", "Академическая", "academic", 5000.0, 80.0, 5, 10, true);
        academic.deactivate();
        REQUIRE_FALSE(academic.isActive());
        REQUIRE_FALSE(academic.isEligible(student, book, journal));

        Scholarship active("SCH-8", "Академическая", "academic", 5000.0, 80.0, 5, 10, true);
        Student expelled = makeStudent("S-2", "expelled");
        REQUIRE_FALSE(active.isEligible(expelled, book, journal));
    }

    SECTION("некорректные данные стипендии") {
        REQUIRE_THROWS_AS(Scholarship("", "Стипендия", "academic", 5000.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Scholarship("SCH-X", "", "academic", 5000.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Scholarship("SCH-X", "Стипендия", "unknown", 5000.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Scholarship("SCH-X", "Стипендия", "academic", 0.0),
                          InsufficientFundsException);
        REQUIRE_THROWS_AS(Scholarship("SCH-X", "Стипендия", "academic", 5000.0, 101.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Scholarship("SCH-X", "Стипендия", "academic", 5000.0, 80.0, -1),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Scholarship("SCH-X", "Стипендия", "academic", 5000.0, 80.0, 0, -1),
                          InvalidStudentDataException);
    }
}

TEST_CASE("ScholarshipAward: утверждение, выплата и отмена", "[finance]") {
    Student student = makeStudent("S-1");
    Semester semester = makeSemester(1);
    Semester otherSemester = makeSemester(2);
    Scholarship scholarship("SCH-1", "Академическая", "academic", 5000.0, 80.0, 2, 10, true);

    SECTION("полный жизненный цикл") {
        ScholarshipAward award("SA-1", &scholarship, &student, &semester, 5000.0, "2026-02-01");
        REQUIRE_FALSE(award.isPaid());
        REQUIRE(award.matchesSemester(semester));
        REQUIRE_FALSE(award.matchesSemester(otherSemester));
        REQUIRE(award.summary().find("SA-1") != std::string::npos);

        award.approve("ORD-1");
        REQUIRE(award.getStatus() == "approved");
        REQUIRE(award.getOrderNumber() == "ORD-1");
        REQUIRE_THROWS_AS(award.approve("ORD-2"), InvalidStudentDataException);

        award.pay();
        REQUIRE(award.isPaid());
        REQUIRE_THROWS_AS(award.pay(), InvalidStudentDataException);
        REQUIRE_THROWS_AS(award.cancel(), InvalidStudentDataException);
    }

    SECTION("отмена ожидающего назначения") {
        ScholarshipAward award("SA-2", &scholarship, &student, &semester, 5000.0, "2026-02-01");
        award.cancel();
        REQUIRE(award.getStatus() == "cancelled");
        REQUIRE_THROWS_AS(award.approve("ORD-1"), InvalidStudentDataException);
    }

    SECTION("некорректные данные назначения") {
        REQUIRE_THROWS_AS(ScholarshipAward("", &scholarship, &student, &semester, 5000.0,
                                          "2026-02-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ScholarshipAward("SA-3", nullptr, &student, &semester, 5000.0,
                                          "2026-02-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ScholarshipAward("SA-3", &scholarship, nullptr, &semester, 5000.0,
                                          "2026-02-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ScholarshipAward("SA-3", &scholarship, &student, nullptr, 5000.0,
                                          "2026-02-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ScholarshipAward("SA-3", &scholarship, &student, &semester, 0.0,
                                          "2026-02-01"),
                          InsufficientFundsException);
        REQUIRE_THROWS_AS(ScholarshipAward("SA-3", &scholarship, &student, &semester, 5000.0,
                                          "01.02.2026"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ScholarshipAward("SA-3", &scholarship, &student, &semester, 5000.0,
                                          "2026-02-01", "approved"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("FinanceAccount: начисления, платежи и история", "[finance]") {
    Student student = makeStudent("S-1");
    Student otherStudent = makeStudent("S-2");
    Specialty specialty = makeSpecialty();
    TuitionContract contract = makeContract(&student, &specialty);
    TuitionContract otherContract = makeContract(&otherStudent, &specialty);
    Teacher teacher = makeTeacher();
    Semester semester = makeSemester();

    SECTION("начисление и долг") {
        FinanceAccount account(&student);
        REQUIRE(account.getStudent() == &student);
        REQUIRE(account.getBalance() == Approx(0.0));
        REQUIRE(account.debt() == Approx(0.0));
        REQUIRE_FALSE(account.hasDebt());
        REQUIRE_THROWS_AS(account.charge(0.0), InsufficientFundsException);
        REQUIRE_THROWS_AS(account.charge(-100.0), InsufficientFundsException);

        account.charge(50000.0);
        REQUIRE(account.getBalance() == Approx(-50000.0));
        REQUIRE(account.debt() == Approx(50000.0));
        REQUIRE(account.hasDebt());
        REQUIRE(account.canPay(20000.0));
        REQUIRE(account.canPay(50000.0));
        REQUIRE_FALSE(account.canPay(50000.01));
        REQUIRE_FALSE(account.canPay(0.0));
        REQUIRE_FALSE(account.canPay(-1.0));
    }

    SECTION("подтверждённый платёж уменьшает долг") {
        FinanceAccount account(&student);
        account.charge(50000.0);

        TuitionPayment pending("TP-1", &contract, 20000.0, "2025-09-10", "card", "R-1");
        REQUIRE_THROWS_AS(account.registerPayment(pending), InvalidStudentDataException);
        pending.confirm(teacher);
        account.registerPayment(pending);
        REQUIRE(account.getBalance() == Approx(-30000.0));
        REQUIRE(account.debt() == Approx(30000.0));
        REQUIRE(account.getTransactions().size() == 1);

        account.registerPayment(pending);
        REQUIRE(account.getTransactions().size() == 1);
        REQUIRE(account.debt() == Approx(30000.0));

        TuitionPayment excessive("TP-2", &contract, 40000.0, "2025-09-11", "card", "R-2");
        excessive.confirm(teacher);
        REQUIRE_THROWS_AS(account.registerPayment(excessive), InsufficientFundsException);
        REQUIRE(account.debt() == Approx(30000.0));

        TuitionPayment foreign("TP-3", &otherContract, 10000.0, "2025-09-12", "card", "R-3");
        foreign.confirm(teacher);
        REQUIRE_THROWS_AS(account.registerPayment(foreign), InvalidStudentDataException);
    }

    SECTION("полное погашение долга") {
        FinanceAccount account(&student);
        account.charge(50000.0);
        TuitionPayment payment("TP-4", &contract, 50000.0, "2025-09-10", "bank_transfer", "R-4");
        payment.confirm(teacher);
        account.registerPayment(payment);
        REQUIRE(account.getBalance() == Approx(0.0));
        REQUIRE(account.debt() == Approx(0.0));
        REQUIRE_FALSE(account.hasDebt());
        REQUIRE_FALSE(account.canPay(1.0));
    }

    SECTION("история платежей за период") {
        FinanceAccount account(&student);
        account.charge(50000.0);
        TuitionPayment first("TP-5", &contract, 20000.0, "2025-09-10", "card", "R-5");
        first.confirm(teacher);
        account.registerPayment(first);
        TuitionPayment second("TP-6", &contract, 10000.0, "2025-10-05", "cash", "R-6");
        second.confirm(teacher);
        account.registerPayment(second);

        REQUIRE(account.historyBetween("2025-09-01", "2025-09-30").size() == 1);
        REQUIRE(account.historyBetween("2025-10-01", "2025-10-31").size() == 1);
        REQUIRE(account.historyBetween("2025-09-01", "2025-10-31").size() == 2);
        REQUIRE(account.historyBetween("2025-11-01", "2025-11-30").empty());
        REQUIRE_THROWS_AS(account.historyBetween("2025-10-01", "2025-09-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(account.historyBetween("вчера", "2025-09-30"),
                          InvalidStudentDataException);
    }

    SECTION("зачисление стипендии") {
        FinanceAccount account(&student);
        account.charge(30000.0);
        Scholarship scholarship("SCH-1", "Академическая", "academic", 5000.0, 80.0, 2, 10, true);

        ScholarshipAward unpaid("SA-1", &scholarship, &student, &semester, 10000.0, "2026-02-01");
        REQUIRE_THROWS_AS(account.applyScholarship(unpaid), InvalidStudentDataException);

        ScholarshipAward paid("SA-2", &scholarship, &student, &semester, 10000.0, "2026-02-01");
        paid.approve("ORD-1");
        paid.pay();
        account.applyScholarship(paid);
        REQUIRE(account.getBalance() == Approx(-20000.0));
        REQUIRE(account.debt() == Approx(20000.0));

        ScholarshipAward foreign("SA-3", &scholarship, &otherStudent, &semester, 10000.0,
                                 "2026-02-01");
        foreign.approve("ORD-2");
        foreign.pay();
        REQUIRE_THROWS_AS(account.applyScholarship(foreign), InvalidStudentDataException);
    }

    SECTION("счёт без студента") {
        REQUIRE_THROWS_AS(FinanceAccount(nullptr), InvalidStudentDataException);
    }
}
