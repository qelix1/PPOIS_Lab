#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "studentis/academic/Assignment.h"
#include "studentis/academic/Classroom.h"
#include "studentis/academic/Course.h"
#include "studentis/academic/CourseMaterial.h"
#include "studentis/academic/Discipline.h"
#include "studentis/academic/Lesson.h"
#include "studentis/academic/Schedule.h"
#include "studentis/academic/ScheduleChange.h"
#include "studentis/academic/Submission.h"
#include "studentis/academic/TeachingLoad.h"
#include "studentis/core/Department.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/ClassroomCapacityException.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/ScheduleConflictException.h"

using namespace studentis;

namespace {

Student makeStudent(const std::string& studentId, const std::string& lastName = "Иванов",
                    const std::string& firstName = "Иван", const std::string& status = "active") {
    return Student("P-" + studentId, lastName, firstName, "Иванович", "2005-09-01", "male",
                   "+79000000000", "student@example.ru", "г. Москва", studentId, "2024-09-01",
                   status, "full_time", "budget", 1);
}

Teacher makeTeacher(const std::string& teacherId, const std::string& position = "доцент") {
    return Teacher("P-" + teacherId, "Петров", "Пётр", "Петрович", "1970-01-01", "male",
                   "+79000000001", "teacher@example.ru", "г. Москва", teacherId, position, "к.т.н.",
                   "", "2010-09-01", 700);
}

StudentGroup makeGroup(const std::string& code, int maxSize = 25) {
    return StudentGroup(code, nullptr, 2026, 1, nullptr, maxSize, "active");
}

Semester makeSemester() {
    return Semester(1, nullptr, "2026-09-01", "2026-12-25", "2026-12-26", "2027-01-15", "active");
}

Classroom makeClassroom(int capacity, const std::string& type = "lecture",
                        bool projector = true, bool computers = false) {
    return Classroom("C-" + type + std::to_string(capacity), "Главный", "101", capacity, type,
                     projector, computers);
}

} // namespace

TEST_CASE("Academic: дисциплина и пререквизиты", "[academic]") {
    Department department("D-1", "Кафедра информационных технологий", nullptr, nullptr, "123-45-67");
    Discipline algorithms("DISC-1", "Алгоритмы и структуры данных", "ALG", &department, 5,
                          "Базовый курс");
    Discipline programming("DISC-2", "Программирование", "PRG", nullptr, 4, "C++");

    SECTION("свойства и поиск по названию и коду") {
        REQUIRE(algorithms.getCredits() == 5);
        REQUIRE(algorithms.summary().find("Алгоритмы") != std::string::npos);
        REQUIRE(algorithms.matchesName("алгоритмы"));
        REQUIRE(algorithms.matchesName("alg"));
        REQUIRE_FALSE(algorithms.matchesName("физика"));
        REQUIRE_FALSE(algorithms.matchesName(""));
    }

    SECTION("добавление пререквизитов без дублей") {
        REQUIRE(algorithms.totalPrerequisites() == 0);
        algorithms.addPrerequisite(programming);
        algorithms.addPrerequisite(programming);
        REQUIRE(algorithms.totalPrerequisites() == 1);
        REQUIRE(algorithms.hasPrerequisite(programming));
        REQUIRE_FALSE(algorithms.hasPrerequisite(algorithms));
    }

    SECTION("дисциплина не может быть своим пререквизитом") {
        REQUIRE_THROWS_AS(algorithms.addPrerequisite(algorithms), InvalidStudentDataException);
    }

    SECTION("закрепление за кафедрой") {
        Discipline physics("DISC-3", "Физика", "PHY", nullptr, 3, "");
        physics.assignDepartment(department);
        REQUIRE(physics.getDepartment() == &department);
        REQUIRE(department.getDisciplines().size() == 1);
        REQUIRE(department.getDisciplines().front() == &physics);
    }

    SECTION("canStudy без пререквизитов и без оценок") {
        Student student = makeStudent("S-1");
        REQUIRE(algorithms.canStudy(student));
        algorithms.addPrerequisite(programming);
        REQUIRE_FALSE(algorithms.canStudy(student));
    }

    SECTION("некорректные данные дисциплины") {
        REQUIRE_THROWS_AS(Discipline("DISC-4", "", "X", nullptr, 1, ""), InvalidStudentDataException);
        REQUIRE_THROWS_AS(Discipline("", "Без кода", "X", nullptr, 1, ""), InvalidStudentDataException);
        REQUIRE_THROWS_AS(Discipline("DISC-5", "Курс", "X", nullptr, -1, ""),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Academic: аудитории", "[academic]") {
    StudentGroup group = makeGroup("G-1", 4);
    Student first = makeStudent("S-1");
    Student second = makeStudent("S-2");
    group.addStudent(first);
    group.addStudent(second);

    SECTION("вместимость и заполненность") {
        Classroom room = makeClassroom(4);
        REQUIRE(room.fits(group));
        REQUIRE(room.utilizationPercent(group) == Catch::Approx(50.0));
        Classroom tiny = makeClassroom(1, "practice", false, false);
        REQUIRE_FALSE(tiny.fits(group));
        REQUIRE(tiny.utilizationPercent(group) == Catch::Approx(100.0));
    }

    SECTION("оборудование под тип занятия") {
        Classroom lectureRoom = makeClassroom(30, "lecture", true, false);
        Classroom plainRoom = makeClassroom(30, "practice", false, false);
        Classroom computerRoom = makeClassroom(30, "lab", false, true);

        Lesson lecture("L-1", nullptr, "2026-09-01", "10:00", "11:30", "lecture", nullptr, "Тема");
        Lesson lab("L-2", nullptr, "2026-09-01", "12:00", "13:30", "lab", nullptr, "Тема");
        Lesson practice("L-3", nullptr, "2026-09-01", "14:00", "15:30", "practice", nullptr, "Тема");

        REQUIRE(lectureRoom.supports(lecture));
        REQUIRE_FALSE(plainRoom.supports(lecture));
        REQUIRE(computerRoom.supports(lab));
        REQUIRE_FALSE(plainRoom.supports(lab));
        REQUIRE(plainRoom.supports(practice));
    }

    SECTION("проверка доступности по занятым интервалам") {
        Classroom room = makeClassroom(30);
        Lesson lesson("L-1", nullptr, "2026-09-01", "10:00", "11:30", "practice", &room, "Тема");
        REQUIRE_FALSE(room.isAvailable("2026-09-01", "11:00", "12:00"));
        REQUIRE_FALSE(room.isAvailable("2026-09-01", "10:00", "11:30"));
        REQUIRE(room.isAvailable("2026-09-01", "11:30", "12:30"));
        REQUIRE(room.isAvailable("2026-09-02", "10:00", "11:30"));
        REQUIRE_FALSE(room.isAvailable("2026-09-01", "12:00", "11:00"));
        REQUIRE_FALSE(room.isAvailable("не дата", "10:00", "11:00"));
    }

    SECTION("тип аудитории и переименование") {
        Classroom room = makeClassroom(30, "lecture", true, false);
        REQUIRE(room.matchesType("lecture"));
        REQUIRE_FALSE(room.matchesType("lab"));
        room.rename("105");
        REQUIRE(room.getRoomNumber() == "105");
        REQUIRE_THROWS_AS(room.rename(""), InvalidStudentDataException);
    }

    SECTION("некорректная вместимость") {
        REQUIRE_THROWS_AS(Classroom("C-X", "Главный", "1", 0, "lecture", true, false),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Academic: занятия и наложение по времени", "[academic]") {
    StudentGroup group = makeGroup("G-1", 10);
    Discipline discipline("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "");
    Classroom room = makeClassroom(10);
    Course course("C-1", &discipline, nullptr, nullptr, &group, 2, &room, "active");

    Lesson first("L-1", &course, "2026-09-01", "10:00", "11:30", "lecture", &room, "Введение");
    Lesson second("L-2", &course, "2026-09-01", "11:00", "12:00", "practice", nullptr, "Практика");
    Lesson third("L-3", &course, "2026-09-01", "12:00", "13:00", "practice", nullptr, "Семинар");

    SECTION("длительность занятия") {
        REQUIRE(first.durationMinutes() == 90);
        REQUIRE(third.durationMinutes() == 60);
    }

    SECTION("пересечение интервалов в один день") {
        REQUIRE(first.overlaps(second));
        REQUIRE_FALSE(first.overlaps(third));
        REQUIRE_FALSE(second.overlaps(third));

        Lesson adjacent("L-4", &course, "2026-09-01", "11:30", "12:30", "practice", nullptr, "Смежное");
        REQUIRE_FALSE(first.overlaps(adjacent));

        Lesson nextDay("L-5", &course, "2026-09-02", "10:00", "11:30", "lecture", nullptr, "Другой день");
        REQUIRE_FALSE(first.overlaps(nextDay));
    }

    SECTION("перенос занятия") {
        first.moveTo("2026-09-03", "09:00", "10:30");
        REQUIRE(first.getDate() == "2026-09-03");
        REQUIRE(first.getStartTime() == "09:00");
        REQUIRE(first.durationMinutes() == 90);
        REQUIRE(first.isOn("2026-09-03"));
        REQUIRE_FALSE(first.isOn("2026-09-01"));
        REQUIRE_THROWS_AS(first.moveTo("2026-09-03", "12:00", "11:00"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(first.moveTo("не дата", "10:00", "11:00"), InvalidStudentDataException);
    }

    SECTION("назначение аудитории с проверкой вместимости") {
        StudentGroup crowded = makeGroup("G-2", 10);
        Student one = makeStudent("S-1");
        Student two = makeStudent("S-2");
        crowded.addStudent(one);
        crowded.addStudent(two);

        Classroom tiny = makeClassroom(1, "practice", false, false);
        Course crowdedCourse("C-2", &discipline, nullptr, nullptr, &crowded, 10, nullptr, "active");
        Lesson lesson("L-6", &crowdedCourse, "2026-09-01", "10:00", "11:30", "practice", nullptr,
                      "Тема");
        REQUIRE_THROWS_AS(lesson.assignClassroom(tiny), ClassroomCapacityException);

        Classroom spacious = makeClassroom(10, "practice", false, false);
        lesson.assignClassroom(spacious);
        REQUIRE(lesson.getClassroom() == &spacious);
    }

    SECTION("отметка о проведении и описание") {
        REQUIRE_FALSE(first.isConducted());
        first.markConducted();
        REQUIRE(first.isConducted());
        REQUIRE(first.isLecture());
        REQUIRE_FALSE(second.isLecture());
        REQUIRE(first.summary().find("Введение") != std::string::npos);
    }

    SECTION("некорректные данные занятия") {
        REQUIRE_THROWS_AS(Lesson("L-X", nullptr, "2026-09-01", "11:00", "10:00", "lecture", nullptr,
                                 "Тема"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Lesson("L-Y", nullptr, "2026-09-01", "10:00", "11:00", "unknown", nullptr,
                                 "Тема"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Lesson("", nullptr, "2026-09-01", "10:00", "11:00", "lecture", nullptr,
                                 "Тема"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Academic: курс и выполнение плана", "[academic]") {
    StudentGroup group = makeGroup("G-1", 10);
    Student first = makeStudent("S-1");
    Student second = makeStudent("S-2");
    group.addStudent(first);
    group.addStudent(second);

    Discipline discipline("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "");
    Teacher teacher = makeTeacher("T-1");
    Classroom room = makeClassroom(10);
    Course course("C-1", &discipline, &teacher, nullptr, &group, 2, &room, "active");

    SECTION("свойства курса") {
        REQUIRE(course.studentsCount() == 2);
        REQUIRE(course.matchesGroup(group));
        StudentGroup other = makeGroup("G-2");
        REQUIRE_FALSE(course.matchesGroup(other));
        REQUIRE(course.getTeacher() == &teacher);
        REQUIRE(course.getPlannedHours() == 2);
    }

    SECTION("назначение преподавателя закрепляет дисциплину") {
        REQUIRE_FALSE(teacher.canTeach(discipline));
        course.assignTeacher(teacher);
        REQUIRE(teacher.canTeach(discipline));
        REQUIRE(course.getTeacher() == &teacher);
    }

    SECTION("назначение аудитории") {
        Classroom tiny = makeClassroom(1, "practice", false, false);
        REQUIRE_THROWS_AS(course.assignClassroom(tiny), ClassroomCapacityException);
        Classroom spacious = makeClassroom(20);
        course.assignClassroom(spacious);
        REQUIRE(course.getClassroom() == &spacious);
    }

    SECTION("добавление занятий и выполнение плана") {
        Lesson lesson("L-1", nullptr, "2026-09-01", "10:00", "11:30", "lecture", &room, "Введение");
        course.addLesson(lesson);
        course.addLesson(lesson);
        REQUIRE(course.getLessons().size() == 1);
        REQUIRE(lesson.getCourse() == &course);
        REQUIRE(course.completionPercent() == Catch::Approx(0.0));
        REQUIRE_FALSE(course.isFinished());

        lesson.markConducted();
        REQUIRE(course.completionPercent() == Catch::Approx(75.0));
        REQUIRE_FALSE(course.isFinished());

        Lesson extra("L-2", nullptr, "2026-09-01", "12:00", "12:30", "practice", &room, "Практика");
        course.addLesson(extra);
        extra.markConducted();
        REQUIRE(course.completionPercent() == Catch::Approx(100.0));
        REQUIRE(course.isFinished());
    }

    SECTION("занятие другого курса не переносится") {
        Lesson lesson("L-1", nullptr, "2026-09-01", "10:00", "11:30", "lecture", &room, "Введение");
        course.addLesson(lesson);
        Course other("C-2", &discipline, nullptr, nullptr, &group, 10, nullptr, "planned");
        REQUIRE_THROWS_AS(other.addLesson(lesson), InvalidStudentDataException);
    }

    SECTION("смена статуса") {
        course.changeStatus("finished");
        REQUIRE(course.getStatus() == "finished");
        REQUIRE(course.isFinished());
        REQUIRE_THROWS_AS(course.changeStatus("закрыт"), InvalidStudentDataException);
    }

    SECTION("некорректные данные курса") {
        REQUIRE_THROWS_AS(Course("", &discipline, nullptr, nullptr, &group, 10, nullptr, "planned"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Course("C-X", &discipline, nullptr, nullptr, &group, -1, nullptr, "planned"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Course("C-Y", &discipline, nullptr, nullptr, &group, 10, nullptr, "bad"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Academic: расписание и конфликты", "[academic]") {
    StudentGroup group = makeGroup("G-1", 10);
    Semester semester = makeSemester();
    Discipline discipline("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "");
    Teacher teacher = makeTeacher("T-1");
    Classroom room = makeClassroom(30);
    Course course("C-1", &discipline, &teacher, &semester, &group, 10, &room, "active");
    Schedule schedule("SCH-1", &group, &semester);

    Lesson first("L-1", &course, "2026-09-01", "10:00", "11:30", "lecture", &room, "Введение");
    Lesson second("L-2", &course, "2026-09-01", "11:00", "12:00", "practice", &room, "Практика");
    Lesson third("L-3", &course, "2026-09-01", "12:00", "13:00", "practice", nullptr, "Семинар");

    SECTION("требование AGENTS: 11:00 конфликтует, 12:00 — нет") {
        schedule.addLesson(first);
        REQUIRE_THROWS_AS(schedule.addLesson(second), ScheduleConflictException);
        schedule.addLesson(third);
        REQUIRE(schedule.getLessons().size() == 2);
        REQUIRE(schedule.totalWeeklyHours() == 2);
    }

    SECTION("занятия по дате и дню недели") {
        schedule.addLesson(first);
        schedule.addLesson(third);
        REQUIRE(schedule.lessonsOn("2026-09-01").size() == 2);
        REQUIRE(schedule.lessonsOn("2026-09-02").empty());
        REQUIRE(schedule.lessonsForWeekday(2).size() == 2);
        REQUIRE(schedule.lessonsForWeekday(1).empty());
        REQUIRE(schedule.lessonsForWeekday(0).empty());
    }

    SECTION("проверка и публикация расписания") {
        schedule.addLesson(first);
        schedule.addLesson(third);
        REQUIRE(schedule.validateNoConflicts());
        schedule.publish();
        REQUIRE(schedule.isPublished());

        third.moveTo("2026-09-01", "11:00", "12:00");
        REQUIRE_FALSE(schedule.validateNoConflicts());
        REQUIRE_THROWS_AS(schedule.publish(), ScheduleConflictException);
    }

    SECTION("поиск свободного окна") {
        schedule.addLesson(first);
        schedule.addLesson(third);
        REQUIRE(schedule.findFreeSlot("2026-09-01", "10:00", 30) == "11:30");
        REQUIRE_THROWS_AS(schedule.findFreeSlot("2026-09-01", "10:00", 0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(schedule.findFreeSlot("не дата", "10:00", 30),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(schedule.findFreeSlot("2026-09-01", "25:00", 30),
                          InvalidStudentDataException);
    }

    SECTION("повторное добавление и удаление") {
        schedule.addLesson(first);
        schedule.addLesson(first);
        REQUIRE(schedule.getLessons().size() == 1);
        REQUIRE(schedule.removeLesson(first));
        REQUIRE_FALSE(schedule.removeLesson(first));
        REQUIRE(schedule.getLessons().empty());
    }

    SECTION("некорректные данные расписания") {
        REQUIRE_THROWS_AS(Schedule("", &group, &semester), InvalidStudentDataException);
    }
}

TEST_CASE("Academic: изменение расписания", "[academic]") {
    StudentGroup group = makeGroup("G-1", 10);
    Discipline discipline("DISC-1", "Алгоритмы", "ALG", nullptr, 5, "");
    Teacher teacher = makeTeacher("T-1");
    Classroom oldRoom = makeClassroom(30);
    Classroom newRoom("C-2", "Главный", "202", 30, "lecture", true, false);
    Course course("C-1", &discipline, &teacher, nullptr, &group, 10, &oldRoom, "active");
    Lesson lesson("L-1", &course, "2026-09-01", "10:00", "11:30", "lecture", &oldRoom, "Введение");

    ScheduleChange change("CH-1", &lesson, "2026-09-01", "2026-09-03", &oldRoom, &newRoom,
                          "болезнь преподавателя", nullptr, "2026-08-30 12:00");

    SECTION("применение требует согласования") {
        REQUIRE_THROWS_AS(change.apply(), InvalidStudentDataException);
    }

    SECTION("согласование и применение") {
        change.approve(teacher);
        REQUIRE(change.getApprovedBy() == &teacher);
        REQUIRE_FALSE(change.isApplied());
        change.apply();
        REQUIRE(lesson.getDate() == "2026-09-03");
        REQUIRE(lesson.getClassroom() == &newRoom);
        REQUIRE(change.isApplied());
        REQUIRE(change.affectsGroup(group));
        REQUIRE(change.summary().find("2026-09-03") != std::string::npos);
    }

    SECTION("отмена возвращает занятие на прежнее место") {
        change.approve(teacher);
        change.apply();
        change.cancel();
        REQUIRE(lesson.getDate() == "2026-09-01");
        REQUIRE(lesson.getClassroom() == &oldRoom);
        REQUIRE_FALSE(change.isApplied());
        REQUIRE(change.getApprovedBy() == nullptr);
    }

    SECTION("некорректные даты изменения") {
        REQUIRE_THROWS_AS(
            ScheduleChange("CH-2", &lesson, "не дата", "2026-09-03", nullptr, nullptr, "причина",
                           nullptr, "2026-08-30"),
            InvalidStudentDataException);
        REQUIRE_THROWS_AS(
            ScheduleChange("", &lesson, "2026-09-01", "2026-09-03", nullptr, nullptr, "причина",
                           nullptr, "2026-08-30"),
            InvalidStudentDataException);
    }
}

TEST_CASE("Academic: нагрузка преподавателя", "[academic]") {
    Teacher teacher = makeTeacher("T-1");
    Semester semester = makeSemester();
    Course course("C-1", nullptr, &teacher, &semester, nullptr, 40, nullptr, "planned");
    TeachingLoad load(&teacher, &semester, 100);

    SECTION("план и проведённые часы") {
        REQUIRE(load.getPlannedHours() == 100);
        REQUIRE(load.getConductedHours() == 0);
        REQUIRE(load.remainingHours() == 100);
        REQUIRE(load.completionPercent() == Catch::Approx(0.0));
        REQUIRE_FALSE(load.isOverloaded());

        load.registerConductedHours(40);
        REQUIRE(load.getConductedHours() == 40);
        REQUIRE(load.remainingHours() == 60);
        REQUIRE(load.completionPercent() == Catch::Approx(40.0));
        REQUIRE(load.overloadHours() == 0);
        REQUIRE_FALSE(load.isOverloaded());
    }

    SECTION("перегрузка") {
        load.registerConductedHours(120);
        REQUIRE(load.remainingHours() == -20);
        REQUIRE(load.overloadHours() == 20);
        REQUIRE(load.isOverloaded());
        REQUIRE(load.completionPercent() == Catch::Approx(120.0));
    }

    SECTION("курсы нагрузки без дублей") {
        load.addCourse(course);
        load.addCourse(course);
        REQUIRE(load.getCourses().size() == 1);
        REQUIRE(load.getCourses().front() == &course);
    }

    SECTION("некорректные часы") {
        REQUIRE_THROWS_AS(load.registerConductedHours(0), InvalidStudentDataException);
        REQUIRE_THROWS_AS(load.registerConductedHours(-5), InvalidStudentDataException);
        REQUIRE_THROWS_AS(TeachingLoad(&teacher, &semester, -1), InvalidStudentDataException);
    }
}

TEST_CASE("Academic: учебные материалы", "[academic]") {
    CourseMaterial material("M-1", nullptr, "Конспект лекций", "link", "https://example.ru/notes",
                            "2026-09-01", 15, true);

    SECTION("тип, обязательность и поиск") {
        REQUIRE(material.isLink());
        REQUIRE(material.isMandatory());
        REQUIRE(material.matchesType("link"));
        REQUIRE_FALSE(material.matchesType("video"));
        REQUIRE(material.summary().find("Конспект") != std::string::npos);
    }

    SECTION("сравнение дат загрузки") {
        REQUIRE(material.olderThan("2026-09-02"));
        REQUIRE_FALSE(material.olderThan("2026-09-01"));
        REQUIRE_FALSE(material.olderThan("не дата"));
    }

    SECTION("переименование") {
        material.rename("Конспект с примерами");
        REQUIRE(material.getTitle() == "Конспект с примерами");
        REQUIRE_THROWS_AS(material.rename(""), InvalidStudentDataException);
    }

    SECTION("некорректные данные материала") {
        REQUIRE_THROWS_AS(CourseMaterial("", nullptr, "Тема", "document", "", "2026-09-01", 1, false),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(CourseMaterial("M-2", nullptr, "", "document", "", "2026-09-01", 1, false),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(CourseMaterial("M-3", nullptr, "Тема", "document", "", "не дата", 1, false),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(CourseMaterial("M-4", nullptr, "Тема", "document", "", "2026-09-01", -1, false),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Academic: задания и сданные работы", "[academic]") {
    Student first = makeStudent("S-1");
    Student second = makeStudent("S-2");
    Teacher teacher = makeTeacher("T-1");
    Assignment assignment("A-1", nullptr, "Лабораторная работа", "Реализовать алгоритм",
                          "2026-09-01", "2026-09-10", 100, "lab");

    SECTION("сдача и проверка работы") {
        Submission work("W-1", &assignment, &first, "2026-09-05", "текст работы");
        assignment.addSubmission(work);
        REQUIRE(assignment.getSubmissions().size() == 1);
        REQUIRE(assignment.hasSubmission(first));
        REQUIRE(assignment.submissionFor(first) == &work);
        REQUIRE_FALSE(assignment.hasSubmission(second));
        REQUIRE(assignment.submissionFor(second) == nullptr);
        REQUIRE_FALSE(work.isGraded());
        REQUIRE(assignment.gradedCount() == 0);
        REQUIRE(assignment.averageScore() == Catch::Approx(0.0));
        REQUIRE(assignment.completionPercent() == Catch::Approx(0.0));

        work.grade(80, teacher);
        REQUIRE(work.isGraded());
        REQUIRE(work.getScore() == 80);
        REQUIRE(work.getCheckedBy() == &teacher);
        REQUIRE(assignment.gradedCount() == 1);
        REQUIRE(assignment.averageScore() == Catch::Approx(80.0));
        REQUIRE(assignment.completionPercent() == Catch::Approx(100.0));
    }

    SECTION("повторная сдача не дублируется") {
        Submission work("W-1", &assignment, &first, "2026-09-05", "текст работы");
        assignment.addSubmission(work);
        assignment.addSubmission(work);
        REQUIRE(assignment.getSubmissions().size() == 1);

        Submission repeat("W-2", &assignment, &first, "2026-09-06", "другая работа");
        assignment.addSubmission(repeat);
        REQUIRE(assignment.getSubmissions().size() == 1);
        REQUIRE(assignment.submissionFor(first) == &work);
    }

    SECTION("балл выше максимального") {
        Submission work("W-1", nullptr, &first, "2026-09-05", "текст работы");
        work.grade(80, teacher);
        Assignment small("A-2", nullptr, "Тест", "Короткая работа", "2026-09-01", "2026-09-10", 50,
                         "test");
        REQUIRE_THROWS_AS(small.addSubmission(work), InvalidGradeException);

        Submission attached("W-3", &small, &second, "2026-09-05", "текст");
        REQUIRE_THROWS_AS(attached.grade(60, teacher), InvalidGradeException);
        attached.grade(50, teacher);
        REQUIRE(attached.getScore() == 50);
    }

    SECTION("срок сдачи и продление") {
        REQUIRE_FALSE(assignment.isOverdue("2026-09-10"));
        REQUIRE(assignment.isOverdue("2026-09-11"));
        REQUIRE_FALSE(assignment.isOverdue("не дата"));

        assignment.extendDeadline("2026-09-15");
        REQUIRE(assignment.getDeadline() == "2026-09-15");
        REQUIRE_THROWS_AS(assignment.extendDeadline("2026-08-31"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(assignment.extendDeadline("не дата"), InvalidStudentDataException);
    }

    SECTION("статусы работы и доработка") {
        Submission work("W-4", &assignment, &second, "2026-09-12", "текст");
        REQUIRE(work.isLate());

        work.returnForRevision("Переделать вывод", teacher);
        REQUIRE(work.needsRevision());
        REQUIRE_FALSE(work.isGraded());
        REQUIRE(work.getFeedback() == "Переделать вывод");
        REQUIRE(work.getCheckedBy() == &teacher);

        work.submit();
        REQUIRE_FALSE(work.needsRevision());
        REQUIRE_THROWS_AS(work.grade(101, teacher), InvalidGradeException);
        REQUIRE_THROWS_AS(work.grade(-1, teacher), InvalidGradeException);
        work.grade(90, teacher);
        REQUIRE(work.isGraded());
        REQUIRE(work.summary().find("90") != std::string::npos);
    }

    SECTION("некорректные данные задания и работы") {
        REQUIRE_THROWS_AS(Assignment("A-3", nullptr, "Тест", "", "2026-09-01", "2026-09-10", 0, "test"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Assignment("A-4", nullptr, "Тест", "", "2026-09-10", "2026-09-01", 100, "test"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Submission("W-5", nullptr, nullptr, "2026-09-05", "текст"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Submission("W-6", nullptr, &first, "не дата", "текст"),
                          InvalidStudentDataException);
    }
}
