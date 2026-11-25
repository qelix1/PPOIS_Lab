#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "studentis/core/Specialty.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/documents/AcademicLeave.h"
#include "studentis/documents/Certificate.h"
#include "studentis/documents/Diploma.h"
#include "studentis/documents/EnrollmentOrder.h"
#include "studentis/documents/PracticeDiary.h"
#include "studentis/documents/StudentCard.h"
#include "studentis/documents/Thesis.h"
#include "studentis/documents/TransferApplication.h"
#include "studentis/exceptions/DocumentExpiredException.h"
#include "studentis/exceptions/GroupNotFoundException.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/StudentNotFoundException.h"

using namespace studentis;

namespace {

Student makeStudent(const std::string& studentId, const std::string& status = "active",
                    const std::string& funding = "budget") {
    return Student("P-" + studentId, "Иванов", "Иван", "Иванович", "2005-09-01", "male",
                   "+79000000000", "student@example.ru", "г. Москва", studentId, "2024-09-01",
                   status, "full_time", funding, 1);
}

Teacher makeTeacher(const std::string& teacherId, const std::string& position = "доцент") {
    return Teacher("P-" + teacherId, "Петров", "Пётр", "Петрович", "1970-01-01", "male",
                   "+79000000001", "teacher@example.ru", "г. Москва", teacherId, position,
                   "к.т.н.", "", "2010-09-01", 700);
}

Specialty makeSpecialty(const std::string& code = "09.03.01",
                        const std::string& name = "Информатика и вычислительная техника") {
    return Specialty(code, name, nullptr, "бакалавр", 4, 200000.0, 25);
}

StudentGroup makeGroup(const std::string& code, int maxSize = 25) {
    return StudentGroup(code, nullptr, 2024, 1, nullptr, maxSize, "active");
}

} // namespace

TEST_CASE("StudentCard: срок действия, статус и доступ", "[documents]") {
    Student student = makeStudent("S-1");
    StudentCard card("C-001", &student, "2020-09-01", "2030-09-01", "active", "1234", "extended");

    SECTION("границы срока действия") {
        REQUIRE(card.isValid("2026-11-25"));
        REQUIRE(card.isValid("2020-09-01"));
        REQUIRE(card.isValid("2030-09-01"));
        REQUIRE_FALSE(card.isValid("2020-08-31"));
        REQUIRE_FALSE(card.isValid("2030-09-02"));
        REQUIRE(card.daysUntilExpiry("2030-08-31") == 1);
        REQUIRE(card.daysUntilExpiry("2030-09-01") == 0);
        REQUIRE(card.daysUntilExpiry("2030-09-02") == -1);
    }

    SECTION("PIN и уровень доступа") {
        REQUIRE(card.verifyPin("1234"));
        REQUIRE_FALSE(card.verifyPin("0000"));
        REQUIRE(card.canEnter("basic", "2026-11-25"));
        REQUIRE(card.canEnter("extended", "2026-11-25"));
        REQUIRE_FALSE(card.canEnter("full", "2026-11-25"));
        REQUIRE_FALSE(card.canEnter("basic", "2031-01-01"));
        REQUIRE_THROWS_AS(card.canEnter("root", "2026-11-25"), InvalidStudentDataException);
    }

    SECTION("просроченный пропуск недействителен") {
        StudentCard expired("C-002", &student, "2020-09-01", "2021-09-01", "active", "1234",
                            "basic");
        REQUIRE_FALSE(expired.isValid("2022-01-01"));
        REQUIRE_THROWS_AS(expired.verifyPin("1234"), DocumentExpiredException);
        REQUIRE_FALSE(expired.canEnter("basic", "2022-01-01"));
    }

    SECTION("блокировка, активация и продление") {
        card.block();
        REQUIRE(card.getStatus() == "blocked");
        REQUIRE_FALSE(card.isValid("2026-11-25"));
        REQUIRE_THROWS_AS(card.verifyPin("1234"), DocumentExpiredException);

        card.activate();
        REQUIRE(card.isValid("2026-11-25"));

        card.renew("2027-09-01", "2031-09-01");
        REQUIRE(card.getStatus() == "active");
        REQUIRE(card.isValid("2028-01-01"));
        REQUIRE_FALSE(card.isValid("2027-08-31"));
        REQUIRE_THROWS_AS(card.renew("2030-01-01", "2029-01-01"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(card.renew("2030-13-01", "2031-01-01"), InvalidStudentDataException);
    }

    SECTION("некорректные данные пропуска") {
        REQUIRE_THROWS_AS(
            StudentCard("", &student, "2020-09-01", "2030-09-01", "active", "1", "basic"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            StudentCard("C-3", nullptr, "2020-09-01", "2030-09-01", "active", "1", "basic"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            StudentCard("C-3", &student, "2030-09-01", "2020-09-01", "active", "1", "basic"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            StudentCard("C-3", &student, "2020-09-01", "2030-09-01", "frozen", "1", "basic"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            StudentCard("C-3", &student, "2020-09-01", "2030-09-01", "active", "", "basic"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            StudentCard("C-3", &student, "2020-09-01", "2030-09-01", "active", "1", "vip"),
            InvalidStudentDataException);
    }
}

TEST_CASE("EnrollmentOrder: подписание и исполнение приказа", "[documents]") {
    Student student = makeStudent("S-1");
    StudentGroup group = makeGroup("G-1");
    Teacher teacher = makeTeacher("T-1");

    SECTION("полный жизненный цикл") {
        EnrollmentOrder order("E-1", &student, &group, "2026-08-25", "15/1", "заявление", nullptr,
                              "draft");
        REQUIRE_FALSE(order.isExecuted());
        REQUIRE(order.isBudget());
        REQUIRE(order.summary().find("G-1") != std::string::npos);
        REQUIRE_THROWS_AS(order.execute(), InvalidStudentDataException);

        order.sign(teacher);
        REQUIRE(order.getStatus() == "signed");
        REQUIRE(order.getSignedBy() == &teacher);
        REQUIRE_THROWS_AS(order.sign(teacher), InvalidStudentDataException);

        order.execute();
        REQUIRE(order.isExecuted());
        REQUIRE(student.getGroup() == &group);
        REQUIRE(group.findStudent("S-1") == &student);
        REQUIRE_THROWS_AS(order.cancel(), InvalidStudentDataException);
    }

    SECTION("отмена черновика") {
        EnrollmentOrder order("E-2", &student, &group, "2026-08-25", "15/2", "заявление", nullptr,
                              "draft");
        order.cancel();
        REQUIRE(order.getStatus() == "cancelled");
        REQUIRE_THROWS_AS(order.execute(), InvalidStudentDataException);
    }

    SECTION("платное обучение и отсутствие группы") {
        Student paid = makeStudent("S-2", "active", "paid");
        EnrollmentOrder order("E-3", &paid, nullptr, "2026-08-25", "15/3", "договор", nullptr,
                              "draft");
        REQUIRE_FALSE(order.isBudget());
        order.sign(teacher);
        REQUIRE_THROWS_AS(order.execute(), GroupNotFoundException);
    }

    SECTION("некорректные данные приказа") {
        REQUIRE_THROWS_AS(EnrollmentOrder("", &student, &group, "2026-08-25", "1", "", nullptr,
                                          "draft"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(EnrollmentOrder("E-4", nullptr, &group, "2026-08-25", "1", "", nullptr,
                                          "draft"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(EnrollmentOrder("E-4", &student, &group, "25.08.2026", "1", "", nullptr,
                                          "draft"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(EnrollmentOrder("E-4", &student, &group, "2026-08-25", "", "", nullptr,
                                          "draft"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(EnrollmentOrder("E-4", &student, &group, "2026-08-25", "1", "", nullptr,
                                          "unknown"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("AcademicLeave: согласование, продление и пересечение", "[documents]") {
    Student student = makeStudent("S-1");
    Teacher teacher = makeTeacher("T-1");

    SECTION("полный жизненный цикл") {
        AcademicLeave leave("L-1", &student, "2026-02-01", "2026-06-30", "болезнь", "5",
                            "pending", nullptr);
        REQUIRE(leave.durationDays() == 150);
        REQUIRE_FALSE(leave.isActive("2026-03-01"));
        REQUIRE_THROWS_AS(leave.extend("2026-07-31"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(leave.close(), InvalidStudentDataException);

        leave.approve(teacher);
        REQUIRE(leave.getApprovedBy() == &teacher);
        REQUIRE(leave.isActive("2026-02-01"));
        REQUIRE(leave.isActive("2026-06-30"));
        REQUIRE_FALSE(leave.isActive("2026-07-01"));

        leave.extend("2026-07-31");
        REQUIRE(leave.getEndDate() == "2026-07-31");
        REQUIRE(leave.durationDays() == 181);
        REQUIRE(leave.isActive("2026-07-15"));
        REQUIRE_THROWS_AS(leave.extend("2026-06-01"), InvalidStudentDataException);

        leave.close();
        REQUIRE(leave.getStatus() == "closed");
        REQUIRE_FALSE(leave.isActive("2026-03-01"));
        REQUIRE_THROWS_AS(leave.approve(teacher), InvalidStudentDataException);
    }

    SECTION("пересечение периодов") {
        AcademicLeave first("L-1", &student, "2026-02-01", "2026-06-30", "болезнь", "5",
                            "approved", &teacher);
        AcademicLeave overlapping("L-2", &student, "2026-06-30", "2026-08-31", "семейные", "6",
                                  "pending", nullptr);
        AcademicLeave later("L-3", &student, "2026-09-01", "2026-12-31", "семейные", "7",
                            "pending", nullptr);
        REQUIRE(first.overlaps(overlapping));
        REQUIRE(overlapping.overlaps(first));
        REQUIRE_FALSE(first.overlaps(later));
    }

    SECTION("некорректные данные отпуска") {
        REQUIRE_THROWS_AS(AcademicLeave("", &student, "2026-02-01", "2026-06-30", "", "1",
                                        "pending", nullptr),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(AcademicLeave("L-1", nullptr, "2026-02-01", "2026-06-30", "", "1",
                                        "pending", nullptr),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(AcademicLeave("L-1", &student, "2026-06-30", "2026-02-01", "", "1",
                                        "pending", nullptr),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(AcademicLeave("L-1", &student, "2026-13-01", "2026-06-30", "", "1",
                                        "pending", nullptr),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(AcademicLeave("L-1", &student, "2026-02-01", "2026-06-30", "", "", "pending",
                                        nullptr),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(AcademicLeave("L-1", &student, "2026-02-01", "2026-06-30", "", "1",
                                        "unknown", nullptr),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Certificate: действие, генерация текста и отзыв", "[documents]") {
    Student student = makeStudent("S-1");
    Teacher teacher = makeTeacher("T-1");

    SECTION("действительная справка") {
        Certificate certificate("C-1", &student, "study", "2026-01-10", &teacher, "для банка",
                                "AB-123", "2026-12-31");
        REQUIRE(certificate.isValid("2026-01-10"));
        REQUIRE(certificate.isValid("2026-12-31"));
        REQUIRE_FALSE(certificate.isValid("2026-01-09"));
        REQUIRE_FALSE(certificate.isValid("2027-01-01"));
        REQUIRE_FALSE(certificate.isForMilitary());
        REQUIRE(certificate.daysUntilExpiry("2026-12-30") == 1);
        REQUIRE(certificate.daysUntilExpiry("2026-12-31") == 0);

        const std::string text = certificate.generateText();
        REQUIRE(text.find("AB-123") != std::string::npos);
        REQUIRE(text.find("Иванов") != std::string::npos);
        REQUIRE(certificate.getText() == text);
    }

    SECTION("военкомат и границы") {
        Certificate military("C-2", &student, "military", "2026-02-01", nullptr, "для военкомата",
                             "ВК-1", "2026-06-30");
        REQUIRE(military.isForMilitary());
        REQUIRE_THROWS_AS(military.daysUntilExpiry("вчера"), InvalidStudentDataException);
    }

    SECTION("истёкшая и отозванная справка") {
        Certificate expired("C-3", &student, "study", "2020-01-10", &teacher, "для банка",
                            "AB-OLD", "2020-12-31");
        REQUIRE(expired.isValid("2020-06-01"));
        REQUIRE_THROWS_AS(expired.generateText(), DocumentExpiredException);

        Certificate revoked("C-4", &student, "study", "2026-01-10", &teacher, "для банка",
                            "AB-4", "2026-12-31");
        revoked.revoke();
        REQUIRE(revoked.getValidUntil().empty());
        REQUIRE_FALSE(revoked.isValid("2026-06-01"));
        REQUIRE(revoked.daysUntilExpiry("2026-06-01") == 0);
        REQUIRE(revoked.summary().find("отозвана") != std::string::npos);
        REQUIRE_THROWS_AS(revoked.generateText(), DocumentExpiredException);
        REQUIRE_THROWS_AS(revoked.revoke(), InvalidStudentDataException);
    }

    SECTION("справка отчисленного студента недействительна") {
        Student expelled = makeStudent("S-2", "expelled");
        Certificate certificate("C-5", &expelled, "study", "2026-01-10", &teacher, "для банка",
                                "AB-5", "2026-12-31");
        REQUIRE_FALSE(certificate.isValid("2026-06-01"));
        REQUIRE_THROWS_AS(certificate.generateText(), DocumentExpiredException);
    }

    SECTION("некорректные данные справки") {
        REQUIRE_THROWS_AS(Certificate("", &student, "study", "2026-01-10", nullptr, "", "1",
                                      "2026-12-31"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Certificate("C-6", nullptr, "study", "2026-01-10", nullptr, "", "1",
                                      "2026-12-31"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Certificate("C-6", &student, "unknown", "2026-01-10", nullptr, "", "1",
                                      "2026-12-31"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Certificate("C-6", &student, "study", "2026-01-10", nullptr, "", "1",
                                      "2026-01-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Certificate("C-6", &student, "study", "2026-01-10", nullptr, "", "",
                                      "2026-12-31"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Diploma: регистрация, отличия и номер", "[documents]") {
    Student student = makeStudent("S-1");
    Specialty specialty = makeSpecialty();

    SECTION("выданный диплом с отличием") {
        Diploma diploma("D-1", &student, &specialty, "123456", "1234", "2026-06-30", 90.0, true,
                        "Разработка системы", 95);
        REQUIRE(diploma.isHonors());
        REQUIRE(diploma.qualification() == "бакалавр");
        REQUIRE(diploma.isIssued());
        REQUIRE(diploma.validateNumber());
        REQUIRE(diploma.matchesSpecialty(specialty));
        REQUIRE(diploma.summary().find("с отличием") != std::string::npos);
        REQUIRE(diploma.summary().find("1234 123456") != std::string::npos);
    }

    SECTION("границы отличия и соответствия") {
        Diploma withoutHonors("D-2", &student, &specialty, "123456", "1234", "2026-06-30", 90.0,
                              false, "", 0);
        REQUIRE_FALSE(withoutHonors.isHonors());

        Diploma lowAverage("D-3", &student, &specialty, "123456", "1234", "2026-06-30", 80.0, true,
                           "", 0);
        REQUIRE_FALSE(lowAverage.isHonors());

        Specialty other = makeSpecialty("01.03.02", "Прикладная математика");
        REQUIRE_FALSE(withoutHonors.matchesSpecialty(other));

        Diploma noSpecialty("D-4", &student, nullptr, "", "", "", 80.0, false, "", 0);
        REQUIRE(noSpecialty.qualification().empty());
        REQUIRE_FALSE(noSpecialty.matchesSpecialty(specialty));
        REQUIRE_FALSE(noSpecialty.isIssued());
        REQUIRE_FALSE(noSpecialty.validateNumber());
        REQUIRE_THROWS_AS(noSpecialty.registerIssue("2026-06-30", "123456", "1234"),
                          InvalidStudentDataException);
    }

    SECTION("регистрация выдачи") {
        Diploma blank("D-5", &student, &specialty, "", "", "", 85.0, false, "", 0);
        REQUIRE_FALSE(blank.isIssued());
        blank.registerIssue("2026-06-30", "654321", "4321");
        REQUIRE(blank.isIssued());
        REQUIRE(blank.getNumber() == "654321");
        REQUIRE(blank.getSeries() == "4321");
        REQUIRE(blank.validateNumber());
        REQUIRE_THROWS_AS(blank.registerIssue("2026-06-30", "12345", "4321"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(blank.registerIssue("2026-06-30", "123456", "43ab"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(blank.registerIssue("30.06.2026", "123456", "4321"),
                          InvalidStudentDataException);
    }

    SECTION("некорректные данные диплома") {
        REQUIRE_THROWS_AS(Diploma("", &student, &specialty, "", "", "", 80.0, false, "", 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Diploma("D-6", nullptr, &specialty, "", "", "", 80.0, false, "", 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Diploma("D-6", &student, &specialty, "12345", "1234", "", 80.0, false,
                                  "", 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Diploma("D-6", &student, &specialty, "", "", "", 101.0, false, "", 0),
                          InvalidGradeException);
        REQUIRE_THROWS_AS(Diploma("D-6", &student, &specialty, "", "", "", 80.0, false, "", 101),
                          InvalidGradeException);
        Diploma diploma("D-7", &student, &specialty, "", "", "", 80.0, false, "", 0);
        REQUIRE_THROWS_AS(diploma.setThesisScore(-1), InvalidGradeException);
        diploma.setThesisScore(88);
        REQUIRE(diploma.getThesisScore() == 88);
    }
}

TEST_CASE("Thesis: плагиат, допуск и защита", "[documents]") {
    Student student = makeStudent("S-1");
    Teacher supervisor = makeTeacher("T-1");

    SECTION("плагиат 15 допускает, 25 не допускает") {
        Thesis thesis("T-1", &student, &supervisor, "Тема", "2026-05-01", "2026-06-01", 15,
                      0, "submitted");
        REQUIRE(thesis.isAllowedToDefend());
        thesis.checkPlagiarism(25);
        REQUIRE_FALSE(thesis.isAllowedToDefend());
        thesis.checkPlagiarism(20);
        REQUIRE(thesis.isAllowedToDefend());
        REQUIRE_THROWS_AS(thesis.checkPlagiarism(101), InvalidStudentDataException);
        REQUIRE_THROWS_AS(thesis.checkPlagiarism(-1), InvalidStudentDataException);
    }

    SECTION("сдача и назначение защиты") {
        Thesis thesis("T-2", &student, &supervisor, "Тема", "", "", 0, 0, "draft");
        REQUIRE_FALSE(thesis.isAllowedToDefend());
        REQUIRE(thesis.daysToDefense("2026-06-01") == 0);
        thesis.setDefenseDate("2026-06-11");
        REQUIRE_FALSE(thesis.isAllowedToDefend());

        thesis.submit("2026-05-01");
        REQUIRE(thesis.getStatus() == "submitted");
        REQUIRE(thesis.isAllowedToDefend());
        REQUIRE(thesis.daysToDefense("2026-06-01") == 10);
        REQUIRE_THROWS_AS(thesis.submit("2026-05-02"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(thesis.submit("вчера"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(thesis.setDefenseDate("2026-04-01"), InvalidStudentDataException);

        Teacher reviewer = makeTeacher("T-2", "профессор");
        thesis.assignReviewer(reviewer);
        REQUIRE(thesis.getSupervisor() == &reviewer);
    }

    SECTION("оценка и статусы") {
        Thesis thesis("T-3", &student, &supervisor, "Тема", "2026-05-01", "2026-06-01", 15, 0,
                      "submitted");
        thesis.grade(85);
        REQUIRE(thesis.getScore() == 85);
        REQUIRE(thesis.getStatus() == "defended");
        REQUIRE(thesis.isPassed());
        REQUIRE_THROWS_AS(thesis.grade(101), InvalidGradeException);

        Thesis failedWork("T-4", &student, &supervisor, "Тема", "2026-05-01", "2026-06-01", 15, 0,
                          "submitted");
        failedWork.grade(59);
        REQUIRE(failedWork.getStatus() == "failed");
        REQUIRE_FALSE(failedWork.isPassed());
        REQUIRE_FALSE(failedWork.isAllowedToDefend());
        REQUIRE_THROWS_AS(failedWork.grade(60), InvalidStudentDataException);
    }

    SECTION("работа с высоким плагиатом не оценивается") {
        Thesis thesis("T-5", &student, &supervisor, "Тема", "2026-05-01", "2026-06-01", 25, 0,
                      "submitted");
        REQUIRE_FALSE(thesis.isAllowedToDefend());
        REQUIRE_THROWS_AS(thesis.grade(90), InvalidStudentDataException);
    }

    SECTION("некорректные данные работы") {
        REQUIRE_THROWS_AS(Thesis("", &student, &supervisor, "Тема", "", "", 0, 0, "draft"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Thesis("T-6", nullptr, &supervisor, "Тема", "", "", 0, 0, "draft"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Thesis("T-6", &student, &supervisor, "", "", "", 0, 0, "draft"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Thesis("T-6", &student, &supervisor, "Тема", "2026-06-01", "2026-05-01", 0, 0, "draft"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Thesis("T-6", &student, &supervisor, "Тема", "", "", 0, 101, "draft"),
            InvalidGradeException);
        REQUIRE_THROWS_AS(
            Thesis("T-6", &student, &supervisor, "Тема", "", "", 0, 0, "unknown"),
            InvalidStudentDataException);
    }
}

TEST_CASE("PracticeDiary: задачи, часы и оценка", "[documents]") {
    Student student = makeStudent("S-1");
    Teacher supervisor = makeTeacher("T-1");

    SECTION("дневник практики") {
        PracticeDiary diary("P-1", &student, &supervisor, "ООО Ромашка", "2026-07-01", "2026-07-14",
                            80, {"Задача 1"}, 0);
        REQUIRE(diary.validateDates());
        REQUIRE(diary.totalHours() == 80);
        REQUIRE(diary.durationDays() == 14);
        REQUIRE(diary.getTasks().size() == 1);
        REQUIRE_FALSE(diary.isComplete());

        diary.addTask("Задача 2");
        REQUIRE(diary.getTasks().size() == 2);
        REQUIRE_THROWS_AS(diary.addTask(""), InvalidStudentDataException);

        diary.rate(75);
        REQUIRE(diary.getFinalScore() == 75);
        REQUIRE(diary.isComplete());
        diary.rate(59);
        REQUIRE_FALSE(diary.isComplete());
        diary.rate(60);
        REQUIRE(diary.isComplete());
        REQUIRE_THROWS_AS(diary.rate(101), InvalidGradeException);

        REQUIRE(diary.summary().find("ООО Ромашка") != std::string::npos);
        REQUIRE(diary.summary().find("80") != std::string::npos);
        REQUIRE_THROWS_AS(diary.setHours(-1), InvalidStudentDataException);
    }

    SECTION("без задач практика не завершена") {
        PracticeDiary diary("P-2", &student, &supervisor, "ООО Ромашка", "2026-07-01", "2026-07-14",
                            80, {}, 90);
        REQUIRE_FALSE(diary.isComplete());
        REQUIRE(diary.getFinalScore() == 90);
    }

    SECTION("некорректные данные дневника") {
        REQUIRE_THROWS_AS(PracticeDiary("", &student, &supervisor, "ООО", "2026-07-01",
                                        "2026-07-14", 80, {}, 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(PracticeDiary("P-3", nullptr, &supervisor, "ООО", "2026-07-01",
                                        "2026-07-14", 80, {}, 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(PracticeDiary("P-3", &student, &supervisor, "", "2026-07-01",
                                        "2026-07-14", 80, {}, 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(PracticeDiary("P-3", &student, &supervisor, "ООО", "2026-07-14",
                                        "2026-07-01", 80, {}, 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(PracticeDiary("P-3", &student, &supervisor, "ООО", "01.07.2026",
                                        "2026-07-14", 80, {}, 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(PracticeDiary("P-3", &student, &supervisor, "ООО", "2026-07-01",
                                        "2026-07-14", -1, {}, 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(PracticeDiary("P-3", &student, &supervisor, "ООО", "2026-07-01",
                                        "2026-07-14", 80, {}, 101),
                          InvalidGradeException);
    }
}

TEST_CASE("TransferApplication: решение и перевод студента", "[documents]") {
    Student student = makeStudent("S-1");
    StudentGroup from = makeGroup("G-1");
    StudentGroup to = makeGroup("G-2");
    from.addStudent(student);
    Teacher teacher = makeTeacher("T-1");

    SECTION("утверждение и применение") {
        TransferApplication application("A-1", &student, &from, &to, "переезд", "2026-11-01",
                                        "pending", nullptr, "");
        REQUIRE(application.isPending());
        REQUIRE(application.isValid());
        REQUIRE(application.summary().find("G-2") != std::string::npos);
        REQUIRE_THROWS_AS(application.apply(), InvalidStudentDataException);

        application.approve(teacher, "2026-11-20");
        REQUIRE(application.getStatus() == "approved");
        REQUIRE(application.getDecidedBy() == &teacher);
        REQUIRE(application.getDecisionDate() == "2026-11-20");

        application.apply();
        REQUIRE(application.getStatus() == "applied");
        REQUIRE(student.getGroup() == &to);
        REQUIRE(from.findStudent("S-1") == nullptr);
        REQUIRE(to.findStudent("S-1") == &student);
        REQUIRE(to.getStudents().size() == 1);
        REQUIRE_THROWS_AS(application.apply(), InvalidStudentDataException);
    }

    SECTION("отклонение заявления") {
        TransferApplication application("A-2", &student, &from, &to, "переезд", "2026-11-01",
                                        "pending", nullptr, "");
        application.reject(teacher, "2026-11-20");
        REQUIRE(application.getStatus() == "rejected");
        REQUIRE_FALSE(application.isPending());
        REQUIRE(student.getGroup() == &from);
        REQUIRE_THROWS_AS(application.apply(), InvalidStudentDataException);
        REQUIRE_THROWS_AS(application.approve(teacher, "2026-11-21"), InvalidStudentDataException);
    }

    SECTION("некорректное заявление не утверждается") {
        TransferApplication wrongSource("A-3", &student, &to, &from, "переезд", "2026-11-01",
                                        "pending", nullptr, "");
        REQUIRE_FALSE(wrongSource.isValid());
        REQUIRE_THROWS_AS(wrongSource.approve(teacher, "2026-11-20"), InvalidStudentDataException);

        TransferApplication sameGroup("A-4", &student, &from, &from, "переезд", "2026-11-01",
                                      "pending", nullptr, "");
        REQUIRE_FALSE(sameGroup.isValid());

        TransferApplication noTarget("A-5", &student, &from, nullptr, "переезд", "2026-11-01",
                                     "pending", nullptr, "");
        REQUIRE_FALSE(noTarget.isValid());
    }

    SECTION("ошибки при исполнении") {
        TransferApplication nullStudent("A-6", nullptr, &from, &to, "переезд", "2026-11-01",
                                        "approved", nullptr, "2026-11-20");
        REQUIRE_THROWS_AS(nullStudent.apply(), StudentNotFoundException);

        TransferApplication noTarget("A-7", &student, &from, nullptr, "переезд", "2026-11-01",
                                     "approved", nullptr, "2026-11-20");
        REQUIRE_THROWS_AS(noTarget.apply(), GroupNotFoundException);
    }

    SECTION("некорректные данные заявления") {
        REQUIRE_THROWS_AS(TransferApplication("", &student, &from, &to, "", "2026-11-01",
                                              "pending", nullptr, ""),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TransferApplication("A-8", &student, &from, &to, "", "01.11.2026",
                                              "pending", nullptr, ""),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TransferApplication("A-8", &student, &from, &to, "", "2026-11-01",
                                              "unknown", nullptr, ""),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(TransferApplication("A-8", &student, &from, &to, "", "2026-11-01",
                                              "approved", nullptr, "20.11.2026"),
                          InvalidStudentDataException);
    }
}
