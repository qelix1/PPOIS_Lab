#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "studentis/academic/Classroom.h"
#include "studentis/academic/Course.h"
#include "studentis/academic/Discipline.h"
#include "studentis/academic/Lesson.h"
#include "studentis/assessment/Attendance.h"
#include "studentis/assessment/AttendanceJournal.h"
#include "studentis/assessment/Exam.h"
#include "studentis/assessment/ExamResult.h"
#include "studentis/assessment/ExamTicket.h"
#include "studentis/assessment/Grade.h"
#include "studentis/assessment/GradeBook.h"
#include "studentis/assessment/Retake.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/AttendanceException.h"
#include "studentis/exceptions/ExamRegistrationException.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

using namespace studentis;

namespace {

Student makeStudent(const std::string& studentId, const std::string& status = "active") {
    return Student("P-" + studentId, "Иванов", "Иван", "Иванович", "2005-09-01", "male",
                   "+79000000000", "student@example.ru", "г. Москва", studentId, "2024-09-01",
                   status, "full_time", "budget", 1);
}

Teacher makeTeacher(const std::string& teacherId = "T-1") {
    return Teacher("P-" + teacherId, "Петров", "Пётр", "Петрович", "1970-01-01", "male",
                   "+79000000001", "teacher@example.ru", "г. Москва", teacherId, "доцент", "к.т.н.",
                   "", "2010-09-01", 700);
}

StudentGroup makeGroup(const std::string& code, int maxSize = 25) {
    return StudentGroup(code, nullptr, 2026, 1, nullptr, maxSize, "active");
}

Semester makeSemester(int number = 1) {
    return Semester(number, nullptr, "2026-09-01", "2026-12-25", "2026-12-26", "2027-01-15", "active");
}

Classroom makeClassroom(int capacity = 30) {
    return Classroom("C-1", "Главный", "101", capacity, "lecture", true, false);
}

} // namespace

TEST_CASE("Assessment: оценка и границы стобалльной шкалы", "[assessment]") {
    Student student = makeStudent("ST-1");
    Teacher teacher = makeTeacher();
    Discipline algorithms("DISC-1", "Алгоритмы и структуры данных", "ALG", nullptr, 5, "Базовый курс");
    Semester semester = makeSemester(1);
    Semester otherSemester = makeSemester(2);

    SECTION("свойства оценки") {
        Grade grade("GR-1", &student, &algorithms, &teacher, 85, "2026-10-01", "exam", &semester,
                    "хорошо");
        REQUIRE(grade.getValue() == 85);
        REQUIRE(grade.isPass());
        REQUIRE(grade.isExcellent());
        REQUIRE_FALSE(grade.isFail());
        REQUIRE(grade.matchesSemester(semester));
        REQUIRE_FALSE(grade.matchesSemester(otherSemester));
        REQUIRE(grade.weight() == Catch::Approx(1.0));
        REQUIRE(grade.getComment() == "хорошо");
        REQUIRE(grade.summary().find("85") != std::string::npos);
        REQUIRE(grade.getStudent() == &student);
        REQUIRE(grade.getDiscipline() == &algorithms);
        REQUIRE(grade.getTeacher() == &teacher);
        REQUIRE(grade.getSemester() == &semester);
    }

    SECTION("граничные значения шкалы") {
        Grade fail("GR-2", &student, &algorithms, &teacher, 59, "2026-10-01", "exam", &semester);
        REQUIRE(fail.isFail());
        REQUIRE_FALSE(fail.isPass());
        REQUIRE_FALSE(fail.isExcellent());
        REQUIRE(fail.summary().find("59") != std::string::npos);

        Grade pass("GR-3", &student, &algorithms, &teacher, 60, "2026-10-01", "exam", &semester);
        REQUIRE(pass.isPass());
        REQUIRE_FALSE(pass.isExcellent());
        REQUIRE_FALSE(pass.isFail());

        Grade almost("GR-4", &student, &algorithms, &teacher, 84, "2026-10-01", "exam", &semester);
        REQUIRE_FALSE(almost.isExcellent());

        Grade excellent("GR-5", &student, &algorithms, &teacher, 85, "2026-10-01", "exam", &semester);
        REQUIRE(excellent.isExcellent());
    }

    SECTION("изменение балла и вес формы контроля") {
        Grade grade("GR-6", &student, &algorithms, &teacher, 70, "2026-10-01", "credit", &semester);
        REQUIRE(grade.weight() == Catch::Approx(0.5));
        grade.updateValue(90);
        REQUIRE(grade.getValue() == 90);
        REQUIRE(grade.isExcellent());

        Grade differentiated("GR-7", &student, &algorithms, &teacher, 75, "2026-10-01",
                             "differentiated_credit", &semester);
        REQUIRE(differentiated.weight() == Catch::Approx(0.8));

        REQUIRE_THROWS_AS(grade.updateValue(-1), InvalidGradeException);
        REQUIRE_THROWS_AS(grade.updateValue(101), InvalidGradeException);
    }

    SECTION("ошибки конструирования оценки") {
        REQUIRE_THROWS_AS(Grade("", &student, &algorithms, &teacher, 80, "2026-10-01", "exam"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Grade("GR-8", nullptr, &algorithms, &teacher, 80, "2026-10-01", "exam"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Grade("GR-9", &student, nullptr, &teacher, 80, "2026-10-01", "exam"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Grade("GR-10", &student, &algorithms, &teacher, 101, "2026-10-01", "exam"),
                          InvalidGradeException);
        REQUIRE_THROWS_AS(Grade("GR-11", &student, &algorithms, &teacher, -5, "2026-10-01", "exam"),
                          InvalidGradeException);
        REQUIRE_THROWS_AS(Grade("GR-12", &student, &algorithms, &teacher, 80, "01.10.2026", "exam"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Grade("GR-13", &student, &algorithms, &teacher, 80, "2026-10-01", "test"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Assessment: экзаменационный билет", "[assessment]") {
    StudentGroup group = makeGroup("G-1");
    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "Базовый курс");
    Teacher teacher = makeTeacher();
    Exam exam("EX-1", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled");

    SECTION("содержимое билета") {
        ExamTicket ticket(1, &exam, "Вопрос 1", "Вопрос 2", "", "hard");
        REQUIRE(ticket.getNumber() == 1);
        REQUIRE(ticket.getExam() == &exam);
        REQUIRE(ticket.questionCount() == 2);
        REQUIRE(ticket.isHard());
        REQUIRE(ticket.matchesNumber(1));
        REQUIRE_FALSE(ticket.matchesNumber(2));
        REQUIRE(ticket.summary().find("2") != std::string::npos);

        ticket.changeDifficulty("easy");
        REQUIRE(ticket.getDifficulty() == "easy");
        REQUIRE_FALSE(ticket.isHard());

        ExamTicket full(2, &exam, "Вопрос 1", "Вопрос 2", "Вопрос 3", "medium");
        REQUIRE(full.questionCount() == 3);
    }

    SECTION("ошибки билета") {
        REQUIRE_THROWS_AS(ExamTicket(0, &exam, "Вопрос", "", "", "easy"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ExamTicket(1, nullptr, "Вопрос", "", "", "easy"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ExamTicket(1, &exam, "", "Вопрос", "", "easy"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ExamTicket(1, &exam, "Вопрос", "", "", "extreme"),
                          InvalidStudentDataException);

        ExamTicket ticket(2, &exam, "Вопрос", "", "", "medium");
        REQUIRE_THROWS_AS(ticket.changeDifficulty("extreme"), InvalidStudentDataException);
    }
}

TEST_CASE("Assessment: экзамен, билеты и регистрация", "[assessment]") {
    Student alice = makeStudent("ST-1");
    Student bob = makeStudent("ST-2");
    Student expelled = makeStudent("ST-3", "expelled");
    StudentGroup group = makeGroup("G-1");
    group.addStudent(alice);
    group.addStudent(bob);
    group.addStudent(expelled);

    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "Базовый курс");
    Teacher teacher = makeTeacher();
    Classroom classroom = makeClassroom();

    SECTION("билеты и готовность к началу") {
        Exam exam("EX-1", &algorithms, &group, &teacher, "2026-12-20", &classroom, 100, "scheduled");
        REQUIRE(exam.getDiscipline() == &algorithms);
        REQUIRE(exam.getGroup() == &group);
        REQUIRE(exam.getTeacher() == &teacher);
        REQUIRE(exam.getClassroom() == &classroom);
        REQUIRE(exam.getMaxScore() == 100);
        REQUIRE_FALSE(exam.canStart());

        ExamTicket first(1, &exam, "Вопрос 1", "Вопрос 2", "Вопрос 3", "medium");
        ExamTicket second(2, &exam, "Вопрос 1", "Вопрос 2", "Вопрос 3", "hard");
        exam.addTicket(first);
        exam.addTicket(first);
        REQUIRE(exam.getTickets().size() == 1);
        REQUIRE(exam.canStart());

        REQUIRE(exam.isScheduledOn("2026-12-20"));
        REQUIRE_FALSE(exam.isScheduledOn("2026-12-21"));
        REQUIRE_FALSE(exam.isScheduledOn("20.12.2026"));

        REQUIRE_FALSE(exam.removeTicket(2));
        exam.addTicket(second);
        REQUIRE(exam.getTickets().size() == 2);
        REQUIRE(exam.removeTicket(1));
        REQUIRE(exam.getTickets().size() == 1);
        REQUIRE(exam.summary().find("EX-1") != std::string::npos);
    }

    SECTION("успешная регистрация и дубли") {
        Exam exam("EX-2", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled");
        REQUIRE(exam.registeredCount() == 0);
        exam.registerStudent(alice);
        REQUIRE(exam.registeredCount() == 1);
        REQUIRE_THROWS_AS(exam.registerStudent(alice), ExamRegistrationException);
        exam.registerStudent(bob);
        REQUIRE(exam.registeredCount() == 2);
    }

    SECTION("неактивный студент не регистрируется") {
        Exam exam("EX-3", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled");
        REQUIRE_THROWS_AS(exam.registerStudent(expelled), ExamRegistrationException);
        REQUIRE(exam.registeredCount() == 0);
    }

    SECTION("студент из другой группы не регистрируется") {
        Student stranger = makeStudent("ST-4");
        StudentGroup other = makeGroup("G-2");
        other.addStudent(stranger);
        Exam exam("EX-4", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled");
        REQUIRE_THROWS_AS(exam.registerStudent(stranger), ExamRegistrationException);
    }

    SECTION("регистрация закрывается после завершения экзамена") {
        Exam exam("EX-5", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled");
        ExamTicket ticket(1, &exam, "Вопрос 1", "Вопрос 2", "Вопрос 3", "medium");
        exam.addTicket(ticket);
        exam.complete();
        REQUIRE(exam.getStatus() == "completed");
        REQUIRE_FALSE(exam.canStart());
        REQUIRE_THROWS_AS(exam.registerStudent(alice), ExamRegistrationException);
        REQUIRE_THROWS_AS(exam.complete(), InvalidStudentDataException);
    }

    SECTION("ошибки конструирования и чужие билеты") {
        REQUIRE_THROWS_AS(
            Exam("", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Exam("EX-6", nullptr, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Exam("EX-7", &algorithms, nullptr, &teacher, "2026-12-20", nullptr, 100, "scheduled"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Exam("EX-8", &algorithms, &group, nullptr, "2026-12-20", nullptr, 100, "scheduled"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Exam("EX-9", &algorithms, &group, &teacher, "20.12.2026", nullptr, 100, "scheduled"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Exam("EX-10", &algorithms, &group, &teacher, "2026-12-20", nullptr, 0, "scheduled"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Exam("EX-11", &algorithms, &group, &teacher, "2026-12-20", nullptr, 101, "scheduled"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            Exam("EX-12", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "unknown"),
            InvalidStudentDataException);

        Exam first("EX-13", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "scheduled");
        Exam second("EX-14", &algorithms, &group, &teacher, "2026-12-21", nullptr, 100, "scheduled");
        ExamTicket foreign(1, &first, "Вопрос", "", "", "easy");
        REQUIRE_THROWS_AS(second.addTicket(foreign), InvalidStudentDataException);
    }

    SECTION("отменённый экзамен нельзя завершить") {
        Exam exam("EX-15", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "cancelled");
        REQUIRE_THROWS_AS(exam.complete(), InvalidStudentDataException);
    }
}

TEST_CASE("Assessment: результат экзамена и пятибалльная шкала", "[assessment]") {
    Student student = makeStudent("ST-1");
    StudentGroup group = makeGroup("G-1");
    group.addStudent(student);
    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "Базовый курс");
    Teacher teacher = makeTeacher();
    Exam exam("EX-1", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "completed");

    SECTION("границы перевода в пятибалльную шкалу") {
        REQUIRE(ExamResult(&exam, &student, 59).toFiveScale() == 2);
        REQUIRE(ExamResult(&exam, &student, 60).toFiveScale() == 3);
        REQUIRE(ExamResult(&exam, &student, 69).toFiveScale() == 3);
        REQUIRE(ExamResult(&exam, &student, 70).toFiveScale() == 4);
        REQUIRE(ExamResult(&exam, &student, 72).toFiveScale() == 4);
        REQUIRE(ExamResult(&exam, &student, 84).toFiveScale() == 4);
        REQUIRE(ExamResult(&exam, &student, 85).toFiveScale() == 5);
        REQUIRE(ExamResult(&exam, &student, 100).toFiveScale() == 5);
    }

    SECTION("сданный и несданный результат") {
        ExamResult failed(&exam, &student, 40, "ответ", "2026-12-21 10:30");
        REQUIRE(failed.getExam() == &exam);
        REQUIRE(failed.getStudent() == &student);
        REQUIRE(failed.getScore() == 40);
        REQUIRE(failed.getGrade() == 2);
        REQUIRE(failed.getAnswers() == "ответ");
        REQUIRE(failed.getCheckedAt() == "2026-12-21 10:30");
        REQUIRE_FALSE(failed.isPassed());
        REQUIRE_FALSE(failed.isExcellent());
        REQUIRE(failed.needsRetake());
        REQUIRE_FALSE(failed.isRetakeAllowed());
        REQUIRE(failed.summary().find("40") != std::string::npos);

        failed.allowRetake();
        REQUIRE(failed.isRetakeAllowed());

        ExamResult excellent(&exam, &student, 90);
        REQUIRE(excellent.isPassed());
        REQUIRE(excellent.isExcellent());
        REQUIRE_FALSE(excellent.needsRetake());
        REQUIRE_THROWS_AS(excellent.allowRetake(), InvalidGradeException);
    }

    SECTION("ошибки результата") {
        REQUIRE_THROWS_AS(ExamResult(nullptr, &student, 80), InvalidStudentDataException);
        REQUIRE_THROWS_AS(ExamResult(&exam, nullptr, 80), InvalidStudentDataException);
        REQUIRE_THROWS_AS(ExamResult(&exam, &student, 101), InvalidGradeException);
        REQUIRE_THROWS_AS(ExamResult(&exam, &student, -1), InvalidGradeException);
        REQUIRE_THROWS_AS(ExamResult(&exam, &student, 80, "", "2026-12-21"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(ExamResult(&exam, &student, 80, "", "21.12.2026 10:00"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Assessment: отметка посещаемости", "[assessment]") {
    Student student = makeStudent("ST-1");
    StudentGroup group = makeGroup("G-1");
    group.addStudent(student);
    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "Базовый курс");
    Teacher teacher = makeTeacher();
    Semester semester = makeSemester();
    Course course("CRS-1", &algorithms, &teacher, &semester, &group, 64, nullptr, "active");
    Lesson lesson("L-1", &course, "2026-10-01", "10:00", "11:30", "lecture", nullptr, "Введение");

    SECTION("статусы и штрафы") {
        Attendance present("A-1", &lesson, &student, "present", 7, "", &teacher);
        REQUIRE(present.isPresent());
        REQUIRE_FALSE(present.isAbsent());
        REQUIRE_FALSE(present.isExcused());
        REQUIRE(present.getMinutesLate() == 0);
        REQUIRE(present.latePenalty() == 0);
        REQUIRE(present.summary().find("present") != std::string::npos);

        Attendance late("A-2", &lesson, &student, "late", 15, "", &teacher);
        REQUIRE(late.isPresent());
        REQUIRE(late.getMinutesLate() == 15);
        REQUIRE(late.latePenalty() == 15);
        REQUIRE(late.summary().find("15") != std::string::npos);

        Attendance veryLate("A-2b", &lesson, &student, "late", 150, "", &teacher);
        REQUIRE(veryLate.latePenalty() == 100);

        Attendance absent("A-3", &lesson, &student, "absent", 0, "", &teacher);
        REQUIRE(absent.isAbsent());
        REQUIRE_FALSE(absent.isPresent());
        REQUIRE(absent.latePenalty() == 100);

        Attendance excused("A-4", &lesson, &student, "excused", 0, "болезнь", &teacher);
        REQUIRE(excused.isExcused());
        REQUIRE(excused.latePenalty() == 0);
        REQUIRE(excused.summary().find("болезнь") != std::string::npos);
    }

    SECTION("уважительная причина") {
        Attendance late("A-5", &lesson, &student, "late", 20, "", &teacher);
        late.markExcused("справка врача");
        REQUIRE(late.isExcused());
        REQUIRE(late.getStatus() == "excused");
        REQUIRE(late.getMinutesLate() == 0);
        REQUIRE(late.getReason() == "справка врача");
        REQUIRE_THROWS_AS(late.markExcused(""), AttendanceException);

        Attendance absent("A-6", &lesson, &student, "absent");
        absent.markExcused("семейные обстоятельства");
        REQUIRE(absent.isExcused());
        REQUIRE(absent.getReason() == "семейные обстоятельства");
    }

    SECTION("ошибки отметки") {
        REQUIRE_THROWS_AS(Attendance("", &lesson, &student, "present"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(Attendance("A-7", nullptr, &student, "present"), AttendanceException);
        REQUIRE_THROWS_AS(Attendance("A-8", &lesson, nullptr, "present"), AttendanceException);
        REQUIRE_THROWS_AS(Attendance("A-9", &lesson, &student, "skipped"), AttendanceException);
        REQUIRE_THROWS_AS(Attendance("A-10", &lesson, &student, "late", -1), AttendanceException);
    }
}

TEST_CASE("Assessment: журнал посещаемости", "[assessment]") {
    Student alice = makeStudent("ST-1");
    Student bob = makeStudent("ST-2");
    Student charlie = makeStudent("ST-3");
    Student dave = makeStudent("ST-4");
    StudentGroup group = makeGroup("G-1");
    group.addStudent(alice);
    group.addStudent(bob);
    group.addStudent(charlie);
    group.addStudent(dave);

    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "Базовый курс");
    Teacher teacher = makeTeacher();
    Semester semester = makeSemester();
    Course course("CRS-1", &algorithms, &teacher, &semester, &group, 64, nullptr, "active");
    Lesson lesson("L-1", &course, "2026-10-01", "10:00", "11:30", "lecture", nullptr, "Введение");

    SECTION("проценты и задолженности по посещению") {
        Attendance present("A-1", &lesson, &alice, "present");
        Attendance late("A-2", &lesson, &dave, "late", 15);
        Attendance absent("A-3", &lesson, &bob, "absent");
        Attendance excused("A-4", &lesson, &charlie, "excused", 0, "болезнь");

        AttendanceJournal journal(&group, &course);
        journal.mark(present);
        journal.mark(late);
        journal.mark(absent);
        journal.mark(excused);

        REQUIRE(journal.getGroup() == &group);
        REQUIRE(journal.getCourse() == &course);
        REQUIRE(journal.getEntries().size() == 4);
        REQUIRE(journal.attendanceRate() == Catch::Approx(75.0));
        REQUIRE(journal.absentCount() == 1);
        REQUIRE(journal.unexcusedAbsences(bob) == 1);
        REQUIRE(journal.unexcusedAbsences(alice) == 0);
        REQUIRE(journal.lessonRate(lesson) == Catch::Approx(75.0));

        std::vector<Student*> chronic = journal.chronicAbsentees(1);
        REQUIRE(chronic.size() == 1);
        REQUIRE(chronic.front() == &bob);
        REQUIRE(journal.chronicAbsentees(2).empty());
        REQUIRE(journal.chronicAbsentees(0).empty());
    }

    SECTION("повторная отметка заменяет прежнюю") {
        Attendance present("A-1", &lesson, &alice, "present");
        Attendance absent("A-2", &lesson, &bob, "absent");
        AttendanceJournal journal(&group, &course);
        journal.mark(present);
        journal.mark(absent);
        REQUIRE(journal.attendanceRate() == Catch::Approx(50.0));

        Attendance corrected("A-3", &lesson, &bob, "present");
        journal.mark(corrected);
        REQUIRE(journal.getEntries().size() == 2);
        REQUIRE(journal.attendanceRate() == Catch::Approx(100.0));
        REQUIRE(journal.absentCount() == 0);
        REQUIRE(journal.unexcusedAbsences(bob) == 0);
    }

    SECTION("пустой журнал") {
        AttendanceJournal journal(&group, &course);
        REQUIRE(journal.attendanceRate() == Catch::Approx(0.0));
        REQUIRE(journal.absentCount() == 0);
        REQUIRE(journal.chronicAbsentees(1).empty());
        REQUIRE(journal.lessonRate(lesson) == Catch::Approx(0.0));
    }

    SECTION("ошибки журнала") {
        REQUIRE_THROWS_AS(AttendanceJournal(nullptr, &course), InvalidStudentDataException);
        REQUIRE_THROWS_AS(AttendanceJournal(&group, nullptr), InvalidStudentDataException);

        AttendanceJournal journal(&group, &course);

        Student outsider = makeStudent("ST-9");
        Attendance foreign("A-5", &lesson, &outsider, "present");
        REQUIRE_THROWS_AS(journal.mark(foreign), AttendanceException);

        Course otherCourse("CRS-2", &algorithms, &teacher, &semester, &group, 32, nullptr, "active");
        Lesson otherLesson("L-2", &otherCourse, "2026-10-02", "10:00", "11:30", "practice", nullptr,
                           "Практика");
        Attendance wrongLesson("A-6", &otherLesson, &alice, "present");
        REQUIRE_THROWS_AS(journal.mark(wrongLesson), AttendanceException);

        AttendanceJournal detached;
        Attendance valid("A-7", &lesson, &alice, "present");
        REQUIRE_THROWS_AS(detached.mark(valid), AttendanceException);
    }
}

TEST_CASE("Assessment: зачётная книжка", "[assessment]") {
    Student student = makeStudent("ST-1");
    Teacher teacher = makeTeacher();
    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "Базовый курс");
    Discipline physics("DISC-2", "Физика", "PHY", nullptr, 4, "Общий курс");
    Discipline chemistry("DISC-3", "Химия", "CHM", nullptr, 3, "Общий курс");
    Semester semester = makeSemester(1);
    Semester second = makeSemester(2);

    SECTION("пустая зачётная книжка") {
        GradeBook book(&student);
        REQUIRE(book.getStudent() == &student);
        REQUIRE(book.getGrades().empty());
        REQUIRE(book.getResults().empty());
        REQUIRE(book.average() == Catch::Approx(0.0));
        REQUIRE(book.averageForSemester(semester) == Catch::Approx(0.0));
        REQUIRE_FALSE(book.hasDebts());
        REQUIRE(book.debtDisciplines().empty());
        REQUIRE(book.excellentCount() == 0);
        REQUIRE(book.ratingScore() == Catch::Approx(0.0));
    }

    SECTION("среднее и производные показатели") {
        GradeBook book(&student);
        Grade first("G-1", &student, &algorithms, &teacher, 80, "2026-10-01", "exam", &semester);
        Grade physicsGrade("G-2", &student, &physics, &teacher, 90, "2026-10-05", "exam", &semester);
        Grade debt("G-3", &student, &chemistry, &teacher, 55, "2026-10-07", "exam", &semester);
        book.addGrade(first);
        book.addGrade(physicsGrade);
        book.addGrade(debt);

        REQUIRE(book.getGrades().size() == 3);
        REQUIRE(book.average() == Catch::Approx(75.0));
        REQUIRE(book.averageForSemester(semester) == Catch::Approx(75.0));
        REQUIRE(book.averageForSemester(second) == Catch::Approx(0.0));
        REQUIRE(book.hasDebts());
        REQUIRE(book.debtDisciplines().size() == 1);
        REQUIRE(book.debtDisciplines().front() == &chemistry);
        REQUIRE(book.excellentCount() == 1);
        REQUIRE(book.ratingScore() == Catch::Approx(72.0));
    }

    SECTION("замена оценки и защита от дублей") {
        GradeBook book(&student);
        Grade failing("G-1", &student, &physics, &teacher, 55, "2026-10-01", "exam", &semester);
        book.addGrade(failing);
        book.addGrade(failing);
        REQUIRE(book.getGrades().size() == 1);

        Grade retake("G-2", &student, &physics, &teacher, 65, "2026-11-01", "exam", &semester);
        book.addGrade(retake);
        REQUIRE(book.getGrades().size() == 1);
        REQUIRE(book.getGrades().front() == &retake);
        REQUIRE(book.average() == Catch::Approx(65.0));
        REQUIRE_FALSE(book.hasDebts());
        REQUIRE(book.debtDisciplines().empty());

        Grade credit("G-3", &student, &physics, &teacher, 75, "2026-11-02", "credit", &semester);
        book.addGrade(credit);
        REQUIRE(book.getGrades().size() == 2);

        Grade nextSemester("G-4", &student, &physics, &teacher, 90, "2027-02-01", "exam", &second);
        book.addGrade(nextSemester);
        REQUIRE(book.getGrades().size() == 3);
        REQUIRE(book.averageForSemester(second) == Catch::Approx(90.0));
    }

    SECTION("границы рейтингового балла") {
        GradeBook low(&student);
        Grade zero("G-5", &student, &algorithms, &teacher, 0, "2026-10-01", "exam", &semester);
        low.addGrade(zero);
        REQUIRE(low.ratingScore() == Catch::Approx(0.0));

        GradeBook high(&student);
        Grade top("G-6", &student, &algorithms, &teacher, 100, "2026-10-01", "exam", &semester);
        high.addGrade(top);
        REQUIRE(high.ratingScore() == Catch::Approx(100.0));
    }

    SECTION("результаты экзаменов и чужие данные") {
        StudentGroup group = makeGroup("G-1");
        group.addStudent(student);
        Exam exam("EX-1", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "completed");

        GradeBook book(&student);
        ExamResult failed(&exam, &student, 45);
        book.addExamResult(failed);
        book.addExamResult(failed);
        REQUIRE(book.getResults().size() == 1);

        ExamResult passed(&exam, &student, 75);
        book.addExamResult(passed);
        REQUIRE(book.getResults().size() == 1);
        REQUIRE(book.getResults().front() == &passed);

        Grade foreign("G-9", &student, &algorithms, &teacher, 80, "2026-10-01", "exam", &semester);
        book.addGrade(foreign);
        REQUIRE(book.getGrades().size() == 1);
    }

    SECTION("ошибки зачётной книжки") {
        REQUIRE_THROWS_AS(GradeBook(nullptr), InvalidStudentDataException);

        GradeBook book(&student);
        Student other = makeStudent("ST-2");
        Grade foreign("G-8", &other, &algorithms, &teacher, 80, "2026-10-01", "exam", &semester);
        REQUIRE_THROWS_AS(book.addGrade(foreign), InvalidStudentDataException);

        StudentGroup group = makeGroup("G-2");
        Exam exam("EX-2", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "completed");
        ExamResult foreignResult(&exam, &other, 80);
        REQUIRE_THROWS_AS(book.addExamResult(foreignResult), InvalidStudentDataException);
    }
}

TEST_CASE("Assessment: пересдача экзамена", "[assessment]") {
    Student student = makeStudent("ST-1");
    Student other = makeStudent("ST-2");
    StudentGroup group = makeGroup("G-1");
    group.addStudent(student);
    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "Базовый курс");
    Teacher teacher = makeTeacher();
    Exam exam("EX-1", &algorithms, &group, &teacher, "2026-12-20", nullptr, 100, "completed");
    ExamResult failing(&exam, &student, 40);
    ExamResult passing(&exam, &student, 90);

    SECTION("полный жизненный цикл") {
        Retake retake("R-1", &failing, &student, "2027-02-01", &teacher, "не сдал экзамен");
        REQUIRE(retake.getId() == "R-1");
        REQUIRE(retake.getResult() == &failing);
        REQUIRE(retake.getStudent() == &student);
        REQUIRE(retake.getScheduledDate() == "2027-02-01");
        REQUIRE(retake.getTeacher() == &teacher);
        REQUIRE(retake.getReason() == "не сдал экзамен");
        REQUIRE(retake.isPending());
        REQUIRE(retake.getStatus() == "pending");
        REQUIRE(retake.canRetake());
        REQUIRE(retake.getNewScore() == -1);
        REQUIRE_THROWS_AS(retake.complete(70), InvalidStudentDataException);

        retake.schedule();
        REQUIRE(retake.getStatus() == "scheduled");
        REQUIRE_FALSE(retake.isPending());
        REQUIRE_THROWS_AS(retake.schedule(), InvalidStudentDataException);
        REQUIRE_THROWS_AS(retake.complete(101), InvalidGradeException);

        retake.complete(45);
        REQUIRE(retake.getStatus() == "completed");
        REQUIRE(retake.getNewScore() == 45);
        REQUIRE_FALSE(retake.isSuccessful());
        REQUIRE_FALSE(retake.canRetake());
        REQUIRE_THROWS_AS(retake.cancel(), InvalidStudentDataException);
    }

    SECTION("успешная пересдача") {
        Retake retake("R-2", &failing, &student, "2027-02-01", &teacher, "не сдал экзамен");
        retake.schedule();
        retake.complete(75);
        REQUIRE(retake.isSuccessful());
        REQUIRE(retake.getNewScore() == 75);
        REQUIRE(retake.getStatus() == "completed");
    }

    SECTION("пересдача сданного экзамена недоступна") {
        Retake retake("R-3", &passing, &student, "2027-02-01", &teacher, "ошибка");
        REQUIRE_FALSE(retake.canRetake());
        REQUIRE_THROWS_AS(retake.schedule(), InvalidGradeException);
    }

    SECTION("отмена пересдачи") {
        Retake retake("R-4", &failing, &student, "2027-02-01", &teacher, "не сдал экзамен");
        retake.cancel();
        REQUIRE(retake.getStatus() == "cancelled");
        REQUIRE_FALSE(retake.isPending());
        REQUIRE_FALSE(retake.canRetake());
        REQUIRE_FALSE(retake.isSuccessful());
        retake.cancel();
        REQUIRE(retake.getStatus() == "cancelled");
        REQUIRE_THROWS_AS(retake.schedule(), InvalidStudentDataException);
    }

    SECTION("ошибки конструирования пересдачи") {
        REQUIRE_THROWS_AS(Retake("", &failing, &student, "2027-02-01", &teacher, "причина"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Retake("R-5", nullptr, &student, "2027-02-01", &teacher, "причина"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Retake("R-6", &failing, nullptr, "2027-02-01", &teacher, "причина"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Retake("R-7", &failing, &student, "01.02.2027", &teacher, "причина"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Retake("R-8", &failing, &student, "2027-02-01", nullptr, "причина"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Retake("R-9", &failing, &other, "2027-02-01", &teacher, "причина"),
                          InvalidStudentDataException);
    }
}
