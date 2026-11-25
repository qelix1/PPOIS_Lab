#include <catch2/catch_test_macros.hpp>

#include "studentis/academic/Discipline.h"
#include "studentis/assessment/Exam.h"
#include "studentis/assessment/Grade.h"
#include "studentis/core/AcademicYear.h"
#include "studentis/core/Curriculum.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/Department.h"
#include "studentis/core/Faculty.h"
#include "studentis/core/Person.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Specialty.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/StudyPlanEntry.h"
#include "studentis/core/Teacher.h"
#include "studentis/core/University.h"
#include "studentis/documents/AcademicLeave.h"
#include "studentis/exceptions/GroupFullException.h"
#include "studentis/exceptions/GroupNotFoundException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/StudentNotFoundException.h"

using namespace studentis;

namespace {

Student makeStudent(const std::string& studentId, const std::string& lastName = "Иванов",
                    const std::string& firstName = "Иван", int courseNumber = 1,
                    const std::string& status = "active", const std::string& funding = "budget") {
    return Student("P-" + studentId, lastName, firstName, "Иванович", "2005-09-01", "male",
                   "+79000000000", "student@example.ru", "г. Москва", studentId, "2024-09-01",
                   status, "full_time", funding, courseNumber);
}

Teacher makeTeacher(const std::string& teacherId, const std::string& position = "доцент",
                    const std::string& academicDegree = "к.т.н.",
                    const std::string& academicTitle = "") {
    return Teacher("P-" + teacherId, "Петров", "Пётр", "Петрович", "1970-01-01", "male",
                   "+79000000001", "teacher@example.ru", "г. Москва", teacherId, position,
                   academicDegree, academicTitle, "2010-09-01", 700);
}

StudentGroup makeGroup(const std::string& code, Specialty* specialty = nullptr, int courseNumber = 1,
                       int maxSize = 25) {
    return StudentGroup(code, specialty, 2024, courseNumber, nullptr, maxSize, "active");
}

} // namespace

TEST_CASE("Person: ФИО, возраст и контакты", "[core]") {
    Person person("P-1", "Иванов", "Иван", "Иванович", "1970-01-01", "male", "+79000000002",
                  "ivanov@example.ru", "г. Москва");

    SECTION("полное и короткое имя") {
        REQUIRE(person.fullName() == "Иванов Иван Иванович");
        REQUIRE(person.shortName() == "Иванов И.И.");
        Person withoutMiddle("P-2", "Петров", "Пётр", "", "1980-05-05", "male", "", "", "");
        REQUIRE(withoutMiddle.fullName() == "Петров Пётр");
        REQUIRE(withoutMiddle.shortName() == "Петров П.");
    }

    SECTION("возраст на дату") {
        REQUIRE(person.ageOn("2026-01-01") == 56);
        REQUIRE(person.ageOn("2025-12-31") == 55);
        REQUIRE(person.ageOn("1970-01-01") == 0);
        REQUIRE(person.isAdultOn("1988-01-01"));
        REQUIRE_FALSE(person.isAdultOn("1987-12-31"));
    }

    SECTION("некорректные данные") {
        REQUIRE_THROWS_AS(Person("P-3", "", "Иван", "", "1970-01-01", "male", "", "", ""),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Person("P-4", "Иванов", "Иван", "", "1970-13-01", "male", "", "", ""),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(person.ageOn("вчера"), InvalidStudentDataException);
    }

    SECTION("контакты и адрес") {
        REQUIRE_THROWS_AS(person.updateContacts("", ""), InvalidStudentDataException);
        person.updateContacts("+79000000003", "");
        REQUIRE(person.getPhone() == "+79000000003");
        REQUIRE(person.getEmail() == "ivanov@example.ru");
        REQUIRE_THROWS_AS(person.changeAddress(""), InvalidStudentDataException);
        person.changeAddress("г. Санкт-Петербург");
        REQUIRE(person.getAddress() == "г. Санкт-Петербург");
    }

    SECTION("поиск по ФИО") {
        REQUIRE(person.matchesName("иванов"));
        REQUIRE(person.matchesName("ИВАН ИВАНОВИЧ"));
        REQUIRE_FALSE(person.matchesName("Петров"));
        REQUIRE_FALSE(person.matchesName(""));
    }
}

TEST_CASE("Student: группа, перевод и статусы", "[core]") {
    SECTION("назначение группы синхронизирует курс") {
        Student student = makeStudent("S-1", "Иванов", "Иван", 1);
        StudentGroup group = makeGroup("ИУ7-11Б", nullptr, 3);
        student.assignGroup(group);
        REQUIRE(student.getGroup() == &group);
        REQUIRE(student.getCourseNumber() == 3);
        REQUIRE(student.isActive());
        REQUIRE(student.isBudget());
    }

    SECTION("перевод в другую группу") {
        Student student = makeStudent("S-1");
        StudentGroup first = makeGroup("ИУ7-11Б");
        StudentGroup second = makeGroup("ИУ7-12Б");
        first.addStudent(student);
        student.transferTo(&second);
        REQUIRE(student.getGroup() == &second);
        REQUIRE(first.findStudent("S-1") == nullptr);
        REQUIRE(second.findStudent("S-1") == &student);
    }

    SECTION("перевод в отсутствующую или заполненную группу") {
        Student student = makeStudent("S-1");
        StudentGroup first = makeGroup("ИУ7-11Б");
        first.addStudent(student);
        REQUIRE_THROWS_AS(student.transferTo(nullptr), GroupNotFoundException);

        Student other = makeStudent("S-2");
        StudentGroup full = makeGroup("ИУ7-13Б", nullptr, 1, 1);
        full.addStudent(other);
        REQUIRE_THROWS_AS(student.transferTo(&full), GroupFullException);
        REQUIRE(student.getGroup() == &first);
    }

    SECTION("отчисление и восстановление") {
        Student student = makeStudent("S-1");
        StudentGroup group = makeGroup("ИУ7-11Б");
        group.addStudent(student);
        student.expel();
        REQUIRE_FALSE(student.isActive());
        REQUIRE(student.getStatus() == "expelled");
        REQUIRE(student.getGroup() == nullptr);
        REQUIRE(group.activeCount() == 0);

        student.restore();
        REQUIRE(student.isActive());
        REQUIRE_THROWS_AS(student.restore(), InvalidStudentDataException);

        Student expelled = makeStudent("S-2", "Иванов", "Иван", 1, "expelled");
        expelled.restore();
        REQUIRE(expelled.isActive());
    }

    SECTION("перевод на следующий курс") {
        Student student = makeStudent("S-1", "Иванов", "Иван", 1);
        student.advanceCourse();
        REQUIRE(student.getCourseNumber() == 2);
        student.setCourseNumber(6);
        REQUIRE_THROWS_AS(student.advanceCourse(), InvalidStudentDataException);
        REQUIRE_THROWS_AS(student.setCourseNumber(0), InvalidStudentDataException);
        REQUIRE_THROWS_AS(student.setCourseNumber(7), InvalidStudentDataException);
    }

    SECTION("неизвестный статус и платная форма") {
        Student student = makeStudent("S-1", "Иванов", "Иван", 1, "active", "contract");
        REQUIRE_THROWS_AS(student.setStatus("неизвестный"), InvalidStudentDataException);
        REQUIRE_FALSE(student.isBudget());
        student.setFunding("budget");
        REQUIRE(student.isBudget());
    }
}

TEST_CASE("StudentGroup: заполнение и состав", "[core]") {
    SECTION("граница заполнения группы") {
        StudentGroup group = makeGroup("ИУ7-11Б", nullptr, 1, 1);
        Student first = makeStudent("S-1");
        Student second = makeStudent("S-2");

        group.addStudent(first);
        REQUIRE(group.activeCount() == 1);
        REQUIRE(group.isFull());
        REQUIRE(group.availableSeats() == 0);
        REQUIRE_THROWS_AS(group.addStudent(second), GroupFullException);
        REQUIRE(group.activeCount() == 1);
    }

    SECTION("повторное добавление не меняет состав") {
        StudentGroup group = makeGroup("ИУ7-11Б", nullptr, 1, 2);
        Student student = makeStudent("S-1");
        group.addStudent(student);
        group.addStudent(student);
        REQUIRE(group.getStudents().size() == 1);
        REQUIRE(group.availableSeats() == 1);
    }

    SECTION("поиск и удаление") {
        StudentGroup group = makeGroup("ИУ7-11Б");
        Student student = makeStudent("S-1");
        group.addStudent(student);
        REQUIRE(group.findStudent("S-1") == &student);
        REQUIRE(group.findStudent("S-404") == nullptr);
        REQUIRE(group.removeStudent("S-1"));
        REQUIRE_FALSE(group.removeStudent("S-1"));
        REQUIRE(group.activeCount() == 0);
    }

    SECTION("курс группы и куратор") {
        StudentGroup group = makeGroup("ИУ7-11Б", nullptr, 1, 10);
        Teacher curator = makeTeacher("T-1");
        group.assignCurator(curator);
        REQUIRE(group.getCurator() == &curator);
        group.advanceCourse();
        REQUIRE(group.getCourseNumber() == 2);
        group.setCourseNumber(6);
        REQUIRE_THROWS_AS(group.advanceCourse(), InvalidStudentDataException);
        REQUIRE_THROWS_AS(makeGroup("", nullptr), InvalidStudentDataException);
        REQUIRE_THROWS_AS(makeGroup("ИУ7-11Б", nullptr, 1, 0), InvalidStudentDataException);
    }
}

TEST_CASE("Faculty и Department: структура", "[core]") {
    Faculty faculty("F-1", "Факультет информатики", "ФИ", nullptr, "+7000", "корпус 1");
    Department department("D-1", "Кафедра ИУ7", nullptr, nullptr, "+7001");

    SECTION("кафедры факультета") {
        faculty.addDepartment(department);
        faculty.addDepartment(department);
        REQUIRE(faculty.getDepartments().size() == 1);
        REQUIRE(department.getFaculty() == &faculty);
        REQUIRE(faculty.findDepartment("D-1") == &department);
        REQUIRE(faculty.findDepartment("Кафедра ИУ7") == &department);
        REQUIRE(faculty.findDepartment("D-404") == nullptr);
    }

    SECTION("преподаватели и степени") {
        Teacher professor = makeTeacher("T-1", "professor", "д.т.н.");
        Teacher assistant = makeTeacher("T-2", "assistant", "");
        department.addTeacher(professor);
        department.addTeacher(assistant);
        department.addTeacher(professor);
        faculty.addDepartment(department);
        REQUIRE(department.getTeachers().size() == 2);
        REQUIRE(professor.getDepartment() == &department);
        REQUIRE(department.teachersWithDegree().size() == 1);
        REQUIRE(faculty.teacherCount() == 2);
        REQUIRE(department.removeTeacher("T-2"));
        REQUIRE_FALSE(department.removeTeacher("T-2"));
        REQUIRE(faculty.teacherCount() == 1);
    }

    SECTION("декан, заведующий и переименование") {
        Teacher dean = makeTeacher("T-1");
        faculty.appointDean(dean);
        department.appointHead(dean);
        REQUIRE(faculty.getDean() == &dean);
        REQUIRE(department.getHead() == &dean);
        faculty.rename("Новый факультет", "");
        REQUIRE(faculty.getName() == "Новый факультет");
        REQUIRE(faculty.getShortName() == "ФИ");
        REQUIRE_THROWS_AS(faculty.rename("", ""), InvalidStudentDataException);
        REQUIRE_THROWS_AS(Department("", "Кафедра", nullptr, nullptr, ""), InvalidStudentDataException);
    }

    SECTION("специальности и студенты факультета") {
        Specialty own("09.03.01", "Информатика", &faculty, "бакалавр", 4, 400000.0, 25);
        Specialty foreign("08.03.01", "Строительство", nullptr, "бакалавр", 4, 300000.0, 10);
        std::vector<Specialty*> specialties{&own, &foreign};
        REQUIRE(faculty.allSpecialties(specialties).size() == 1);

        StudentGroup group = makeGroup("ИУ7-11Б", &own);
        Student student = makeStudent("S-1");
        group.addStudent(student);
        std::vector<Student*> students{&student};
        REQUIRE(faculty.studentCount(students) == 1);
        student.expel();
        REQUIRE(faculty.studentCount(students) == 0);
    }
}

TEST_CASE("Specialty и Curriculum: учебный план", "[core]") {
    Faculty faculty("F-1", "Факультет информатики", "ФИ", nullptr, "+7000", "корпус 1");
    Specialty specialty("09.03.01", "Информатика и вычислительная техника", &faculty, "бакалавр",
                        4, 400000.0, 25);

    SECTION("свойства специальности") {
        REQUIRE_FALSE(specialty.isLongProgram());
        REQUIRE(specialty.annualFee() == 100000.0);
        REQUIRE(specialty.durationMonths() == 40);
        REQUIRE(specialty.hasBudgetSeats());
        REQUIRE(specialty.matchesCode("09.03.01"));
        specialty.rename("Прикладная информатика");
        REQUIRE(specialty.getName() == "Прикладная информатика");
        REQUIRE_THROWS_AS(specialty.rename(""), InvalidStudentDataException);

        Specialty longProgram("31.05.01", "Лечебное дело", &faculty, "специалист", 6, 600000.0, 0);
        REQUIRE(longProgram.isLongProgram());
        REQUIRE(longProgram.annualFee() == 100000.0);
        REQUIRE_FALSE(longProgram.hasBudgetSeats());
    }

    SECTION("создание учебного плана") {
        Curriculum plan = specialty.createCurriculum("CUR-2", 2025);
        REQUIRE(plan.getSpecialty() == &specialty);
        REQUIRE(plan.getAdmissionYear() == 2025);
        REQUIRE_FALSE(plan.getApprovedAt().empty());
        REQUIRE_THROWS_AS(Curriculum("", &specialty, 2025, "2025-06-01"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(Curriculum("CUR-3", &specialty, 2025, "01.06.2025"),
                          InvalidStudentDataException);
    }

    SECTION("пересчёт кредитов и состав плана") {
        Curriculum plan("CUR-1", &specialty, 2024, "2024-06-01");
        StudyPlanEntry first(nullptr, 1, 3, 32, 16, 16, "exam", true);
        StudyPlanEntry second(nullptr, 2, 4, 32, 32, 0, "credit", true);
        StudyPlanEntry third(nullptr, 3, 5, 16, 16, 16, "differentiated_credit", false);

        plan.addEntry(first);
        plan.addEntry(second);
        plan.addEntry(third);
        REQUIRE(plan.getEntries().size() == 3);
        REQUIRE(plan.getTotalCredits() == 12);
        REQUIRE(plan.recalcCredits() == 12);
        REQUIRE(plan.creditsForSemester(1) == 3);
        REQUIRE(plan.creditsForSemester(2) == 4);
        REQUIRE(plan.creditsForSemester(4) == 0);
        REQUIRE(plan.requiredDisciplines().size() == 2);
        REQUIRE(plan.electiveDisciplines().size() == 1);
        REQUIRE(plan.findEntry(nullptr, 2) == &second);
        REQUIRE(plan.findEntry(nullptr, 5) == nullptr);
        REQUIRE_FALSE(plan.validate());
    }

    SECTION("повторная и некорректная строка плана") {
        Curriculum plan("CUR-1", &specialty, 2024, "2024-06-01");
        StudyPlanEntry entry(nullptr, 1, 3, 32, 16, 16, "exam", true);
        StudyPlanEntry duplicate(nullptr, 1, 2, 16, 0, 0, "credit", false);
        plan.addEntry(entry);
        plan.addEntry(duplicate);
        REQUIRE(plan.getEntries().size() == 1);
        REQUIRE(plan.getTotalCredits() == 3);

        StudyPlanEntry invalid(nullptr, 1, 0, 16, 0, 0, "credit", false);
        REQUIRE_THROWS_AS(plan.addEntry(invalid), InvalidStudentDataException);

        REQUIRE(plan.removeEntry(nullptr, 1));
        REQUIRE_FALSE(plan.removeEntry(nullptr, 1));
        REQUIRE(plan.getTotalCredits() == 0);
    }
}

TEST_CASE("StudyPlanEntry: часы и форма контроля", "[core]") {
    SECTION("обязательная дисциплина с экзаменом") {
        StudyPlanEntry entry(nullptr, 1, 3, 30, 20, 10, "exam", true);
        REQUIRE(entry.totalHours() == 60);
        REQUIRE(entry.isExam());
        REQUIRE_FALSE(entry.isCredit());
        REQUIRE_FALSE(entry.isElective());
        REQUIRE(entry.matchesSemester(1));
        REQUIRE_FALSE(entry.matchesSemester(2));
        REQUIRE(entry.validateHours());
    }

    SECTION("дисциплина по выбору") {
        StudyPlanEntry entry(nullptr, 2, 2, 16, 16, 0, "credit", false);
        REQUIRE(entry.isCredit());
        REQUIRE(entry.isElective());
        entry.changeControlForm("differentiated_credit");
        REQUIRE_FALSE(entry.isCredit());
        REQUIRE_THROWS_AS(entry.changeControlForm("зачёт"), InvalidStudentDataException);
    }

    SECTION("некорректные часы и семестр") {
        REQUIRE_THROWS_AS(StudyPlanEntry(nullptr, 0, 3, 30, 20, 10, "exam", true),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(StudyPlanEntry(nullptr, 1, 3, -1, 20, 10, "exam", true),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(StudyPlanEntry(nullptr, 1, 3, 30, 20, 10, "зачёт", true),
                          InvalidStudentDataException);
        StudyPlanEntry emptyCredits(nullptr, 1, 0, 30, 20, 10, "exam", true);
        REQUIRE_FALSE(emptyCredits.validateHours());
    }
}

TEST_CASE("AcademicYear и Semester: даты и сессия", "[core]") {
    AcademicYear year("2025/2026", "2025-09-01", "2026-06-30", true);

    SECTION("интервал учебного года") {
        REQUIRE(year.isCurrent());
        REQUIRE(year.containsDate("2026-01-15"));
        REQUIRE_FALSE(year.containsDate("2027-01-01"));
        REQUIRE_FALSE(year.containsDate("не дата"));
        REQUIRE(year.durationDays() == 303);
        REQUIRE_THROWS_AS(AcademicYear("2026/2027", "2026-09-01", "2026-06-30", false),
                          InvalidStudentDataException);
    }

    SECTION("семестры учебного года") {
        Semester semester(1, nullptr, "2025-09-01", "2025-12-31", "2026-01-10", "2026-01-25",
                          "active");
        year.addSemester(semester);
        year.addSemester(semester);
        REQUIRE(year.getSemesters().size() == 1);
        REQUIRE(semester.getYear() == &year);
        REQUIRE(year.findSemester(1) == &semester);
        REQUIRE(year.findSemester(2) == nullptr);
    }

    SECTION("свойства семестра") {
        Semester semester(1, &year, "2025-09-01", "2025-12-31", "2026-01-10", "2026-01-25",
                          "active");
        REQUIRE(semester.containsDate("2025-10-01"));
        REQUIRE_FALSE(semester.containsDate("2026-01-15"));
        REQUIRE(semester.isExamPeriod("2026-01-15"));
        REQUIRE_FALSE(semester.isExamPeriod("2025-10-01"));
        REQUIRE(semester.weeksCount() == 17);
        REQUIRE(semester.isActive());
        REQUIRE(semester.daysUntilExams("2026-01-01") == 9);
        REQUIRE(semester.daysUntilExams("2026-01-15") == 0);
        REQUIRE_THROWS_AS(semester.daysUntilExams("завтра"), InvalidStudentDataException);
    }

    SECTION("закрытие года и семестра") {
        Semester semester(1, &year, "2025-09-01", "2025-12-31", "2026-01-10", "2026-01-25",
                          "active");
        year.addSemester(semester);
        year.close();
        REQUIRE_FALSE(year.isCurrent());
        REQUIRE_FALSE(semester.isActive());
        REQUIRE(semester.getStatus() == "closed");
        REQUIRE_FALSE(semester.canEditGrades());
    }

    SECTION("некорректный семестр") {
        REQUIRE_THROWS_AS(Semester(0, &year, "2025-09-01", "2025-12-31", "2026-01-10",
                                   "2026-01-25", "active"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Semester(1, &year, "2025-12-31", "2025-09-01", "2026-01-10",
                                   "2026-01-25", "active"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("Teacher: степень, звание и стаж", "[core]") {
    SECTION("стаж и кафедра") {
        Teacher teacher = makeTeacher("T-1");
        REQUIRE_FALSE(teacher.isProfessor());
        REQUIRE(teacher.yearsOfService("2020-09-01") == 10);
        REQUIRE(teacher.yearsOfService("2020-08-31") == 9);
        REQUIRE(teacher.yearsOfService("2009-01-01") == 0);
        Department department("D-1", "Кафедра ИУ7", nullptr, nullptr, "+7001");
        department.addTeacher(teacher);
        REQUIRE(teacher.getDepartment() == &department);
        REQUIRE_THROWS_AS(teacher.yearsOfService("вчера"), InvalidStudentDataException);
    }

    SECTION("профессорское звание") {
        Teacher byPosition = makeTeacher("T-1", "professor", "д.т.н.");
        Teacher byTitle = makeTeacher("T-2", "доцент", "", "professor");
        REQUIRE(byPosition.isProfessor());
        REQUIRE(byTitle.isProfessor());
    }

    SECTION("некорректные данные") {
        REQUIRE_THROWS_AS(Teacher("P-9", "Петров", "Пётр", "", "1970-01-01", "male", "", "", "", "",
                                  "доцент", "", "", "2010-09-01", 700),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Teacher("P-9", "Петров", "Пётр", "", "1970-01-01", "male", "", "", "",
                                  "T-9", "доцент", "", "", "01.09.2010", 700),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Teacher("P-9", "Петров", "Пётр", "", "1970-01-01", "male", "", "", "",
                                  "T-9", "доцент", "", "", "2010-09-01", -1),
                          InvalidStudentDataException);
    }
}

TEST_CASE("University: факультеты, студенты и год", "[core]") {
    SECTION("факультеты") {
        University university("МГТУ им. Баумана", nullptr, 1830);
        Faculty faculty("F-1", "Факультет информатики", "ФИ", nullptr, "+7000", "корпус 1");
        university.addFaculty(faculty);
        university.addFaculty(faculty);
        REQUIRE(university.getFaculties().size() == 1);
        REQUIRE(university.findFaculty("F-1") == &faculty);
        REQUIRE(university.findFaculty("Факультет информатики") == &faculty);
        REQUIRE(university.findFaculty("F-404") == nullptr);
    }

    SECTION("зачисление и отчисление") {
        University university("МГТУ им. Баумана", nullptr, 1830);
        Student student = makeStudent("S-1");
        StudentGroup group = makeGroup("ИУ7-11Б");
        group.addStudent(student);
        university.enrollStudent(student);
        university.enrollStudent(student);
        REQUIRE(university.getStudents().size() == 1);
        REQUIRE(university.totalStudents() == 1);
        REQUIRE(university.findGroup("ИУ7-11Б") == &group);
        REQUIRE(university.findGroup("ИУ7-404") == nullptr);

        university.expelStudent("S-1");
        REQUIRE(university.totalStudents() == 0);
        REQUIRE_FALSE(student.isActive());
        REQUIRE_THROWS_AS(university.expelStudent("S-404"), StudentNotFoundException);
    }

    SECTION("текущий учебный год и сводка") {
        University university("МГТУ им. Баумана", nullptr, 1830);
        AcademicYear first("2024/2025", "2024-09-01", "2025-06-30", false);
        AcademicYear second("2025/2026", "2025-09-01", "2026-06-30", false);
        university.setCurrentYear(first);
        REQUIRE(first.isCurrent());
        university.setCurrentYear(second);
        REQUIRE_FALSE(first.isCurrent());
        REQUIRE(second.isCurrent());
        REQUIRE(university.getCurrentYear() == &second);
        REQUIRE(university.summary().find("МГТУ им. Баумана") != std::string::npos);
        REQUIRE(university.summary().find("2025/2026") != std::string::npos);
    }

    SECTION("некорректные данные университета") {
        REQUIRE_THROWS_AS(University("", nullptr, 1830), InvalidStudentDataException);
        REQUIRE_THROWS_AS(University("МГТУ", nullptr, 100), InvalidStudentDataException);
    }
}

TEST_CASE("Department и Teacher: дисциплины и нагрузка", "[core]") {
    Department department("D-1", "Кафедра ИУ7", nullptr, nullptr, "+7001");
    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", &department, 4, "Базовый курс");
    Teacher teacher = makeTeacher("T-1");

    department.assignDiscipline(algorithms);
    REQUIRE(department.findDiscipline("DISC-1") == &algorithms);
    REQUIRE(department.findDiscipline("DISC-404") == nullptr);
    REQUIRE(department.totalPlannedWorkload() == 144);

    algorithms.assignDepartment(department);
    REQUIRE(algorithms.getDepartment() == &department);

    teacher.assignDiscipline(algorithms);
    REQUIRE(teacher.getDisciplines().size() == 1);
    REQUIRE(teacher.plannedWorkload() == 144);
    REQUIRE(teacher.workloadPercent() == 21);
    teacher.setBaseWorkloadHours(0);
    REQUIRE(teacher.workloadPercent() == 0);
    teacher.removeDiscipline(algorithms);
    REQUIRE_FALSE(teacher.canTeach(algorithms));
}

TEST_CASE("DateUtils: календарная строгость", "[core]") {
    REQUIRE(date::isDate("2024-02-29"));
    REQUIRE(date::isDate("2000-02-29"));
    REQUIRE_FALSE(date::isDate("2023-02-29"));
    REQUIRE_FALSE(date::isDate("1900-02-29"));
    REQUIRE_FALSE(date::isDate("2026-02-30"));
    REQUIRE_FALSE(date::isDate("2026-13-01"));
    REQUIRE_FALSE(date::isDate("вчера"));
    REQUIRE(date::daysBetween("2026-01-01", "2026-01-11") == 10);
    REQUIRE(date::minutesOfDateTime("2026-09-01 10:30") == 630);
    REQUIRE(date::minutesOfDateTime("2026-09-01") == -1);
}

TEST_CASE("Student и Grade: средний балл, долги и допуск", "[core]") {
    Teacher teacher = makeTeacher("T-1");
    StudentGroup group = makeGroup("ИУ7-11Б");
    Student student = makeStudent("S-1");
    group.addStudent(student);
    Discipline algorithms("DISC-1", "Алгоритмы", "ALG", nullptr, 4, "");
    Discipline databases("DISC-2", "Базы данных", "DB", nullptr, 4, "");
    databases.addPrerequisite(algorithms);

    Grade good("G-1", &student, &algorithms, &teacher, 80, "2026-01-10", "exam");
    student.recordGrade(good);
    student.recordGrade(good);
    REQUIRE(student.getGrades().size() == 1);
    REQUIRE(student.averageGrade() == 80.0);
    REQUIRE(student.gradesForDiscipline(algorithms).size() == 1);
    REQUIRE_FALSE(student.hasAcademicDebt());
    REQUIRE(group.averageScore() == 80.0);
    REQUIRE(group.studentsWithDebts().empty());

    Exam exam("E-1", &databases, &group, &teacher, "2026-01-20", nullptr, 100);
    REQUIRE(student.canTakeExam(exam));

    Grade fail("G-2", &student, &databases, &teacher, 40, "2026-01-12", "exam");
    student.recordGrade(fail);
    REQUIRE(student.hasAcademicDebt());
    REQUIRE(group.studentsWithDebts().size() == 1);
    student.expel();
    REQUIRE_FALSE(student.canTakeExam(exam));

    Student leaveStudent = makeStudent("S-2");
    AcademicLeave leave =
        leaveStudent.requestAcademicLeave("2026-02-01", "2026-06-30", "семейные обстоятельства");
    REQUIRE(leaveStudent.getStatus() == "academic_leave");
    REQUIRE(leave.getStudent() == &leaveStudent);
    REQUIRE(leave.durationDays() == 150);
    REQUIRE_THROWS_AS(
        leaveStudent.requestAcademicLeave("2026-07-01", "2026-08-01", "повторно"),
        InvalidStudentDataException);
}
