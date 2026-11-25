#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "studentis/academic/Classroom.h"
#include "studentis/academic/Course.h"
#include "studentis/academic/Discipline.h"
#include "studentis/academic/Lesson.h"
#include "studentis/assessment/Attendance.h"
#include "studentis/assessment/AttendanceJournal.h"
#include "studentis/assessment/Grade.h"
#include "studentis/assessment/GradeBook.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Student.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/DocumentExpiredException.h"
#include "studentis/exceptions/DormitoryFullException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/services/Book.h"
#include "studentis/services/BookLoan.h"
#include "studentis/services/Dormitory.h"
#include "studentis/services/DormRoom.h"
#include "studentis/services/Event.h"
#include "studentis/services/LibraryCard.h"
#include "studentis/services/Notification.h"
#include "studentis/services/NotificationService.h"
#include "studentis/services/ProgressReport.h"
#include "studentis/services/StudentDirectory.h"

using namespace studentis;

namespace {

Student makeStudent(const std::string& studentId, const std::string& gender = "male",
                    const std::string& status = "active", const std::string& lastName = "Иванов",
                    int courseNumber = 1, const std::string& funding = "budget") {
    return Student("P-" + studentId, lastName, "Иван", "Иванович", "2005-09-01", gender,
                   "+79000000000", "student@example.ru", "г. Москва", studentId, "2024-09-01",
                   status, "full_time", funding, courseNumber);
}

Teacher makeTeacher(const std::string& teacherId = "T-1") {
    return Teacher("P-" + teacherId, "Петров", "Пётр", "Петрович", "1970-01-01", "male",
                   "+79000000001", "teacher@example.ru", "г. Москва", teacherId, "доцент", "к.т.н.",
                   "", "2010-09-01", 700);
}

StudentGroup makeGroup(const std::string& code, int maxSize = 25) {
    return StudentGroup(code, nullptr, 2024, 1, nullptr, maxSize, "active");
}

Semester makeSemester(int number = 1) {
    return Semester(number, nullptr, "2026-09-01", "2026-12-25", "2026-12-26", "2027-01-15",
                    "active");
}

Classroom makeClassroom(int capacity = 30) {
    return Classroom("C-1", "Главный", "101", capacity, "lecture", true, false);
}

Discipline makeDiscipline(const std::string& id = "D-1") {
    return Discipline(id, "Алгоритмы и структуры данных", "ALG", nullptr, 5, "Базовый курс");
}

} // namespace

TEST_CASE("Book: наличие экземпляров и поиск по выходным данным", "[services]") {
    SECTION("выдача и возврат экземпляров") {
        Book book("B-1", "Война и мир", "Лев Толстой", "978-5-0000-0000-1", 2020, 2, 2, "fiction");
        REQUIRE(book.isAvailable());
        REQUIRE(book.borrow());
        REQUIRE(book.getCopiesAvailable() == 1);
        REQUIRE(book.borrow());
        REQUIRE_FALSE(book.isAvailable());
        REQUIRE_FALSE(book.borrow());
        REQUIRE_THROWS_AS(book.reserve(), InvalidStudentDataException);

        REQUIRE(book.returnCopy());
        REQUIRE(book.getCopiesAvailable() == 1);
        REQUIRE(book.returnCopy());
        REQUIRE_FALSE(book.returnCopy());
        REQUIRE(book.getCopiesAvailable() == book.getCopiesTotal());
    }

    SECTION("резервирование уменьшает число свободных экземпляров") {
        Book book("B-2", "Преступление и наказание", "Фёдор Достоевский", "978-5-0000-0000-2",
                  2019, 1, 1, "classic");
        book.reserve();
        REQUIRE_FALSE(book.isAvailable());
        REQUIRE_THROWS_AS(book.reserve(), InvalidStudentDataException);
    }

    SECTION("поиск без учёта регистра и новизна") {
        Book book("B-3", "Мастер и Маргарита", "Михаил Булгаков", "978-5-0000-0000-3", 2022, 3, 3,
                  "classic");
        REQUIRE(book.matchesTitle("мастер"));
        REQUIRE(book.matchesTitle("МАРГАРИТА"));
        REQUIRE_FALSE(book.matchesTitle(""));
        REQUIRE(book.matchesAuthor("булгаков"));
        REQUIRE_FALSE(book.matchesAuthor("Толстой"));
        REQUIRE(book.isNew(2026));
        REQUIRE(book.isNew(2027));
        REQUIRE_FALSE(book.isNew(2028));
        REQUIRE_FALSE(book.isNew(2021));
    }

    SECTION("некорректные данные книги") {
        REQUIRE_THROWS_AS(Book("", "Книга", "Автор", "ISBN", 2020, 1, 1),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Book("B-4", "", "Автор", "ISBN", 2020, 1, 1),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Book("B-5", "Книга", "", "ISBN", 2020, 1, 1),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Book("B-6", "Книга", "Автор", "ISBN", 0, 1, 1),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Book("B-7", "Книга", "Автор", "ISBN", 2020, 1, 2),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Book("B-8", "Книга", "Автор", "ISBN", 2020, -1, 0),
                          InvalidStudentDataException);
    }
}

TEST_CASE("BookLoan: просрочка, штраф и возврат", "[services]") {
    Student student = makeStudent("S-1");
    Book book("B-1", "Чистый код", "Роберт Мартин", "978-5-0000-0000-4", 2021, 2, 2, "technical");

    SECTION("штраф 50 за пять дней просрочки") {
        BookLoan loan("L-1", &book, &student, "2025-12-20", "2026-01-10", 10.0);
        REQUIRE(loan.isActive());
        REQUIRE_FALSE(loan.isOverdue("2026-01-10"));
        REQUIRE(loan.overdueDays("2026-01-10") == 0);
        REQUIRE(loan.calculateFine("2026-01-10") == Catch::Approx(0.0));
        REQUIRE(loan.isOverdue("2026-01-11"));
        REQUIRE(loan.overdueDays("2026-01-11") == 1);
        REQUIRE(loan.calculateFine("2026-01-11") == Catch::Approx(10.0));
        REQUIRE(loan.overdueDays("2026-01-15") == 5);
        REQUIRE(loan.calculateFine("2026-01-15") == Catch::Approx(50.0));
        REQUIRE(loan.calculateFine("2026-01-20") == Catch::Approx(100.0));
    }

    SECTION("возврат закрывает выдачу и освобождает экземпляр") {
        REQUIRE(book.borrow());
        REQUIRE(book.getCopiesAvailable() == 1);
        BookLoan loan("L-2", &book, &student, "2025-12-20", "2026-01-10", 10.0);
        loan.returnBook("2026-01-15");
        REQUIRE(loan.getStatus() == "returned");
        REQUIRE(loan.getReturnedAt() == "2026-01-15");
        REQUIRE_FALSE(loan.isActive());
        REQUIRE(loan.overdueDays("2026-01-20") == 0);
        REQUIRE(loan.calculateFine("2026-01-20") == Catch::Approx(0.0));
        REQUIRE(book.getCopiesAvailable() == 2);
        REQUIRE_THROWS_AS(loan.returnBook("2026-01-16"), InvalidStudentDataException);
    }

    SECTION("продление срока") {
        BookLoan loan("L-3", &book, &student, "2025-12-20", "2026-01-10", 10.0);
        loan.extend("2026-01-20");
        REQUIRE(loan.getDueDate() == "2026-01-20");
        REQUIRE_FALSE(loan.isOverdue("2026-01-15"));
        REQUIRE_THROWS_AS(loan.extend("2026-01-20"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(loan.extend("2026-01-01"), InvalidStudentDataException);
        loan.returnBook("2026-01-21");
        REQUIRE_THROWS_AS(loan.extend("2026-02-01"), InvalidStudentDataException);
    }

    SECTION("некорректные данные выдачи") {
        REQUIRE_THROWS_AS(BookLoan("", &book, &student, "2025-12-20", "2026-01-10", 10.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(BookLoan("L-4", nullptr, &student, "2025-12-20", "2026-01-10", 10.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(BookLoan("L-5", &book, nullptr, "2025-12-20", "2026-01-10", 10.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(BookLoan("L-6", &book, &student, "2026-01-11", "2026-01-10", 10.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(BookLoan("L-7", &book, &student, "2025-12-20", "2026-01-10", -1.0),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(BookLoan("L-8", &book, &student, "2025-12-20", "2026-01-10", 10.0, "",
                                   "lost"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("LibraryCard: действие билета, выдачи и задолженность", "[services]") {
    Student student = makeStudent("S-1");
    Book book("B-1", "Дюна", "Фрэнк Герберт", "978-5-0000-0000-5", 2020, 2, 2, "fiction");

    SECTION("границы срока действия билета") {
        LibraryCard card("LC-1", &student, "2025-09-01", "2026-09-01");
        REQUIRE(card.isValid("2025-09-01"));
        REQUIRE(card.isValid("2026-09-01"));
        REQUIRE_FALSE(card.isValid("2025-08-31"));
        REQUIRE_FALSE(card.isValid("2026-09-02"));

        card.renew("2027-09-01");
        REQUIRE(card.getExpiresAt() == "2027-09-01");
        REQUIRE(card.isValid("2027-01-01"));
        REQUIRE_THROWS_AS(card.renew("2027-09-01"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(card.renew("2027-13-01"), InvalidStudentDataException);
    }

    SECTION("просроченный и заблокированный билет") {
        LibraryCard expired("LC-2", &student, "2025-09-01", "2025-12-31");
        BookLoan loan("L-1", &book, &student, "2026-01-05", "2026-02-01", 10.0);
        REQUIRE_FALSE(expired.isValid("2026-01-05"));
        REQUIRE_THROWS_AS(expired.addLoan(loan), DocumentExpiredException);

        LibraryCard blocked("LC-3", &student, "2025-09-01", "2026-09-01", "blocked");
        REQUIRE_FALSE(blocked.isValid("2026-01-05"));
        REQUIRE_THROWS_AS(blocked.renew("2027-09-01"), DocumentExpiredException);

        Student expelled = makeStudent("S-2", "male", "expelled");
        LibraryCard card("LC-4", &expelled, "2025-09-01", "2026-09-01");
        REQUIRE_FALSE(card.isValid("2026-01-05"));
    }

    SECTION("выдачи, просрочка и штраф") {
        LibraryCard card("LC-5", &student, "2025-09-01", "2026-09-01");
        BookLoan loan("L-2", &book, &student, "2026-01-05", "2026-01-10", 10.0);
        card.addLoan(loan);
        card.addLoan(loan);
        REQUIRE(card.getLoans().size() == 1);
        REQUIRE(card.activeLoans().size() == 1);
        REQUIRE(card.hasOverdue("2026-01-09") == false);
        REQUIRE(card.hasOverdue("2026-01-15"));
        REQUIRE(card.debtFine("2026-01-15") == Catch::Approx(50.0));

        card.returnLoan("L-2", "2026-01-15");
        REQUIRE(loan.getStatus() == "returned");
        REQUIRE(card.activeLoans().empty());
        REQUIRE(card.hasOverdue("2026-01-20") == false);
        REQUIRE(card.debtFine("2026-01-20") == Catch::Approx(0.0));
        REQUIRE_THROWS_AS(card.returnLoan("L-404", "2026-01-20"), InvalidStudentDataException);
    }

    SECTION("чужая и повторная выдача") {
        LibraryCard card("LC-6", &student, "2025-09-01", "2026-09-01");
        Student other = makeStudent("S-3");
        BookLoan foreign("L-3", &book, &other, "2026-01-05", "2026-01-10", 10.0);
        REQUIRE_THROWS_AS(card.addLoan(foreign), InvalidStudentDataException);
        REQUIRE_THROWS_AS(LibraryCard("", &student, "2025-09-01", "2026-09-01"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(LibraryCard("LC-7", nullptr, "2025-09-01", "2026-09-01"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("DormRoom: места, пол и стоимость проживания", "[services]") {
    Teacher warden = makeTeacher();
    Dormitory dormitory("D-1", "ул. Студенческая, 1", 5, &warden, 5000.0, "+79000000002");
    DormRoom room("101", &dormitory, 2, 1, "male");
    dormitory.addRoom(room);

    SECTION("одно место из двух занято — 50 процентов") {
        Student first = makeStudent("S-1");
        dormitory.assignStudent(first);
        REQUIRE(room.getResidents().size() == 1);
        REQUIRE(room.freeBeds() == 1);
        REQUIRE(room.hasFreeBed());
        REQUIRE_FALSE(room.isFull());
        REQUIRE(dormitory.occupancyRate() == Catch::Approx(50.0));
        REQUIRE(dormitory.freeBeds() == 1);
        REQUIRE(dormitory.monthlyRevenue() == Catch::Approx(5000.0));
        REQUIRE(room.monthlyPrice() == Catch::Approx(5000.0));
    }

    SECTION("третий жилец не помещается") {
        Student first = makeStudent("S-1");
        Student second = makeStudent("S-2");
        Student third = makeStudent("S-3");
        dormitory.assignStudent(first);
        dormitory.assignStudent(second);
        REQUIRE(room.isFull());
        REQUIRE(dormitory.occupancyRate() == Catch::Approx(100.0));
        REQUIRE_THROWS_AS(dormitory.assignStudent(third), DormitoryFullException);
        REQUIRE_THROWS_AS(room.addResident(third), DormitoryFullException);
    }

    SECTION("дубль и пол комнаты") {
        Student first = makeStudent("S-1");
        room.addResident(first);
        room.addResident(first);
        REQUIRE(room.getResidents().size() == 1);
        REQUIRE(room.containsStudent(first));

        Student female = makeStudent("S-4", "female");
        DormRoom femaleRoom("102", &dormitory, 2, 1, "female");
        REQUIRE_THROWS_AS(femaleRoom.addResident(first), DormitoryFullException);
        femaleRoom.addResident(female);
        REQUIRE(femaleRoom.containsStudent(female));

        DormRoom mixed("103", &dormitory, 3, 2, "mixed");
        mixed.addResident(first);
        mixed.addResident(female);
        REQUIRE(mixed.getResidents().size() == 2);
    }

    SECTION("выселение") {
        Student first = makeStudent("S-1");
        dormitory.assignStudent(first);
        REQUIRE(dormitory.evictStudent("S-1"));
        REQUIRE_FALSE(dormitory.evictStudent("S-1"));
        REQUIRE(room.getResidents().empty());
        REQUIRE(dormitory.occupancyRate() == Catch::Approx(0.0));
        REQUIRE(dormitory.monthlyRevenue() == Catch::Approx(0.0));
        REQUIRE_FALSE(room.removeResident("S-1"));
    }

    SECTION("некорректные данные комнаты и общежития") {
        REQUIRE_THROWS_AS(DormRoom("", &dormitory, 2, 1, "male"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(DormRoom("104", &dormitory, 0, 1, "male"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(DormRoom("105", &dormitory, 2, 0, "male"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(DormRoom("106", &dormitory, 2, 1, "unknown"), InvalidStudentDataException);
        REQUIRE_THROWS_AS(Dormitory("D-2", "Адрес", 0, &warden, 5000.0, ""),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Dormitory("D-3", "Адрес", 1, &warden, 0.0, ""),
                          InvalidStudentDataException);

        DormRoom closed("108", &dormitory, 2, 1, "male", "closed");
        Student student = makeStudent("S-5");
        REQUIRE_THROWS_AS(closed.addResident(student), InvalidStudentDataException);
    }
}

TEST_CASE("Dormitory: комнаты, поиск места и лимит комнат", "[services]") {
    Teacher warden = makeTeacher();
    Dormitory dormitory("D-1", "ул. Студенческая, 1", 2, &warden, 4500.0, "+79000000002");
    DormRoom maleRoom("201", &dormitory, 1, 2, "male");
    DormRoom femaleRoom("202", &dormitory, 1, 2, "female");
    dormitory.addRoom(maleRoom);
    dormitory.addRoom(femaleRoom);
    dormitory.addRoom(maleRoom);

    REQUIRE(dormitory.getRooms().size() == 2);
    REQUIRE(dormitory.findFreeRoom("male") == &maleRoom);
    REQUIRE(dormitory.findFreeRoom("female") == &femaleRoom);
    REQUIRE(dormitory.findFreeRoom("mixed") == &maleRoom);

    Student male = makeStudent("S-1");
    Student female = makeStudent("S-2", "female");
    Student extra = makeStudent("S-3");
    dormitory.assignStudent(male);
    dormitory.assignStudent(female);
    REQUIRE(dormitory.freeBeds() == 0);
    REQUIRE_THROWS_AS(dormitory.assignStudent(extra), DormitoryFullException);

    Student inactive = makeStudent("S-4", "male", "expelled");
    REQUIRE_THROWS_AS(dormitory.assignStudent(inactive), InvalidStudentDataException);

    Dormitory tiny("D-2", "Адрес", 1, nullptr, 4000.0, "");
    DormRoom only("301", &tiny, 1, 1, "mixed");
    tiny.addRoom(only);
    DormRoom second("302", &tiny, 1, 1, "mixed");
    REQUIRE_THROWS_AS(tiny.addRoom(second), InvalidStudentDataException);
}

TEST_CASE("Notification: отправка, прочтение и канал", "[services]") {
    Student student = makeStudent("S-1");

    SECTION("жизненный цикл уведомления") {
        Notification notification("N-1", &student, "Зачёт", "Зачёт перенесён", "2026-11-20", "email",
                                  "pending", "high");
        REQUIRE(notification.getStatus() == "pending");
        REQUIRE_FALSE(notification.isRead());
        REQUIRE(notification.isUrgent());
        REQUIRE(notification.matchesChannel("email"));
        REQUIRE_FALSE(notification.matchesChannel("sms"));
        REQUIRE(notification.summary().find("Зачёт") != std::string::npos);
        REQUIRE(notification.daysOld("2026-11-25") == 5);
        REQUIRE(notification.daysOld("2026-11-19") == 0);

        REQUIRE_THROWS_AS(notification.markRead("2026-11-21"), InvalidStudentDataException);
        REQUIRE(notification.send());
        REQUIRE(notification.getStatus() == "sent");
        notification.markRead("2026-11-21");
        REQUIRE(notification.isRead());
        REQUIRE(notification.getReadAt() == "2026-11-21");
        notification.markRead("2026-11-22");
        REQUIRE(notification.getReadAt() == "2026-11-21");
        REQUIRE(notification.send());
        REQUIRE(notification.isRead());
        Notification sentOnly("N-9", &student, "Заголовок", "Текст", "2026-11-20", "email", "sent");
        REQUIRE_THROWS_AS(sentOnly.markRead("2026-13-01"), InvalidStudentDataException);
    }

    SECTION("неудачная отправка и приоритеты") {
        Notification failed("N-2", nullptr, "Заголовок", "Текст", "2026-11-20", "push");
        REQUIRE_FALSE(failed.send());
        REQUIRE(failed.getStatus() == "failed");
        REQUIRE_THROWS_AS(failed.markRead("2026-11-21"), InvalidStudentDataException);

        Notification untitled("N-3", &student, "", "Текст", "2026-11-20", "sms");
        REQUIRE_FALSE(untitled.send());
        REQUIRE(untitled.getStatus() == "failed");

        Notification normal("N-4", &student, "Обычное", "Текст", "2026-11-20", "messenger");
        REQUIRE_FALSE(normal.isUrgent());
    }

    SECTION("некорректные данные уведомления") {
        REQUIRE_THROWS_AS(Notification("", &student, "T", "X", "2026-11-20", "email"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Notification("N-5", &student, "T", "X", "20.11.2026", "email"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Notification("N-6", &student, "T", "X", "2026-11-20", "fax"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Notification("N-7", &student, "T", "X", "2026-11-20", "email", "new"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Notification("N-8", &student, "T", "X", "2026-11-20", "email", "sent",
                                       "critical"),
                          InvalidStudentDataException);
    }
}

TEST_CASE("NotificationService: доставка, прочтение и очистка", "[services]") {
    Student first = makeStudent("S-1");
    Student second = makeStudent("S-2", "female");
    Student outsider = makeStudent("S-3");
    Student inactive = makeStudent("S-4", "male", "expelled");
    StudentGroup group = makeGroup("IS-21");
    group.addStudent(first);
    group.addStudent(second);
    group.addStudent(inactive);

    SECTION("доставка одному студенту и доля доставок") {
        NotificationService service;
        REQUIRE(service.deliveryRate() == Catch::Approx(0.0));

        Notification delivered("N-1", &first, "Заголовок", "Текст", "2026-11-20", "email");
        REQUIRE(service.notifyStudent(delivered));
        REQUIRE(service.getSentCount() == 1);
        REQUIRE(service.deliveryRate() == Catch::Approx(100.0));
        REQUIRE(service.notifyStudent(delivered));
        REQUIRE(service.getSentCount() == 1);
        REQUIRE(service.getNotifications().size() == 1);
        REQUIRE(service.pendingCount() == 0);

        Notification failed("N-2", nullptr, "Заголовок", "Текст", "2026-11-20", "push");
        REQUIRE_FALSE(service.notifyStudent(failed));
        REQUIRE(service.getFailedCount() == 1);
        REQUIRE(service.deliveryRate() == Catch::Approx(50.0));

        REQUIRE(service.unreadFor(first).size() == 1);
        REQUIRE(service.unreadFor(second).empty());
        service.markAllRead("2026-11-21");
        REQUIRE(delivered.isRead());
        REQUIRE(service.unreadFor(first).empty());
    }

    SECTION("рассылка по группе") {
        NotificationService service;
        Notification one("N-1", &first, "Первое", "Текст", "2026-11-20", "email");
        Notification two("N-2", &second, "Второе", "Текст", "2026-11-20", "sms");
        Notification three("N-3", &outsider, "Третье", "Текст", "2026-11-20", "push");
        Notification four("N-4", &inactive, "Четвёртое", "Текст", "2026-11-20", "email");
        const std::vector<Notification*> batch{&one, &two, &three, &four};
        REQUIRE(service.notifyGroup(group, batch) == 2);
        REQUIRE(one.getStatus() == "sent");
        REQUIRE(two.getStatus() == "sent");
        REQUIRE(three.getStatus() == "pending");
        REQUIRE(four.getStatus() == "pending");
        REQUIRE(service.getSentCount() == 2);
    }

    SECTION("очистка старых уведомлений") {
        NotificationService service;
        Notification old("N-1", &first, "Старое", "Текст", "2026-11-01", "email");
        Notification fresh("N-2", &first, "Новое", "Текст", "2026-11-24", "email");
        REQUIRE(service.notifyStudent(old));
        REQUIRE(service.notifyStudent(fresh));
        REQUIRE(service.purgeOlderThan(10, "2026-11-25") == 1);
        REQUIRE(service.getNotifications().size() == 1);
        REQUIRE(service.purgeOlderThan(1, "2026-11-25") == 0);
        REQUIRE_THROWS_AS(service.purgeOlderThan(-1, "2026-11-25"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(service.purgeOlderThan(1, "25.11.2026"), InvalidStudentDataException);
    }
}

TEST_CASE("Event: регистрация, посещение и конфликты", "[services]") {
    Teacher teacher = makeTeacher();
    Classroom classroom = makeClassroom();

    SECTION("регистрация и границы участников") {
        Event event("E-1", "Конференция", "Ежегодная конференция", "2026-12-01 10:00",
                    "2026-12-01 12:00", &teacher, &classroom, 2, "planned");
        Student first = makeStudent("S-1");
        Student second = makeStudent("S-2");
        Student third = makeStudent("S-3");
        Student inactive = makeStudent("S-4", "male", "expelled");

        REQUIRE(event.durationMinutes() == 120);
        REQUIRE(event.isUpcoming("2026-11-25"));
        REQUIRE(event.isUpcoming("2026-12-01"));
        REQUIRE_FALSE(event.isUpcoming("2026-12-02"));
        REQUIRE(event.registerStudent(first));
        REQUIRE_FALSE(event.registerStudent(first));
        REQUIRE(event.registerStudent(second));
        REQUIRE(event.isFull());
        REQUIRE_FALSE(event.registerStudent(third));
        REQUIRE_FALSE(event.registerStudent(inactive));
        REQUIRE(event.getParticipants().size() == 2);

        REQUIRE(event.unregister("S-2"));
        REQUIRE_FALSE(event.unregister("S-2"));
        REQUIRE(event.registerStudent(third));
        REQUIRE(event.isFull());
    }

    SECTION("посещаемость") {
        Event event("E-2", "Семинар", "Семинар", "2026-12-01 10:00", "2026-12-01 12:00", &teacher,
                    &classroom, 3, "planned");
        Student first = makeStudent("S-1");
        Student second = makeStudent("S-2");
        Student outsider = makeStudent("S-3");
        REQUIRE(event.registerStudent(first));
        REQUIRE(event.registerStudent(second));
        REQUIRE(event.attendancePercent() == Catch::Approx(0.0));
        event.markAttended(first);
        event.markAttended(first);
        REQUIRE(event.getAttended().size() == 1);
        REQUIRE(event.attendancePercent() == Catch::Approx(50.0));
        REQUIRE_THROWS_AS(event.markAttended(outsider), InvalidStudentDataException);
        REQUIRE(event.unregister("S-1"));
        REQUIRE(event.getAttended().empty());
        REQUIRE(event.attendancePercent() == Catch::Approx(0.0));
    }

    SECTION("отмена и конфликт по времени") {
        Event event("E-3", "Лекция", "Лекция", "2026-12-01 10:00", "2026-12-01 12:00", &teacher,
                    &classroom, 5, "planned");
        Student student = makeStudent("S-1");
        REQUIRE(event.registerStudent(student));
        event.cancel();
        REQUIRE(event.getStatus() == "cancelled");
        REQUIRE_FALSE(event.registerStudent(student));
        REQUIRE_THROWS_AS(event.markAttended(student), InvalidStudentDataException);

        Event overlapping("E-4", "Кружок", "Кружок", "2026-12-01 11:00", "2026-12-01 13:00",
                          &teacher, &classroom, 5, "planned");
        Event adjacent("E-5", "Кружок", "Кружок", "2026-12-01 12:00", "2026-12-01 13:00", &teacher,
                       &classroom, 5, "planned");
        Event nextDay("E-6", "Кружок", "Кружок", "2026-12-02 10:00", "2026-12-02 12:00", &teacher,
                      &classroom, 5, "planned");
        Event base("E-7", "База", "База", "2026-12-01 10:00", "2026-12-01 12:00", &teacher,
                   &classroom, 5, "planned");
        REQUIRE(base.conflictsWith(overlapping));
        REQUIRE_FALSE(base.conflictsWith(adjacent));
        REQUIRE_FALSE(base.conflictsWith(nextDay));

        Event completed("E-8", "Итоги", "Итоги", "2026-12-01 10:00", "2026-12-01 12:00", &teacher,
                        &classroom, 5, "completed");
        REQUIRE_FALSE(completed.isUpcoming("2026-11-01"));
        REQUIRE_THROWS_AS(completed.cancel(), InvalidStudentDataException);
    }

    SECTION("некорректные данные мероприятия") {
        REQUIRE_THROWS_AS(Event("", "Название", "", "2026-12-01 10:00", "2026-12-01 12:00",
                                &teacher, &classroom, 5),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Event("E-9", "", "", "2026-12-01 10:00", "2026-12-01 12:00", &teacher,
                                &classroom, 5),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Event("E-10", "Название", "", "2026-12-01 12:00", "2026-12-01 10:00",
                                &teacher, &classroom, 5),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(Event("E-11", "Название", "", "2026-12-01 10:00", "2026-12-01 12:00",
                                &teacher, &classroom, 0),
                          InvalidStudentDataException);
    }
}

TEST_CASE("ProgressReport: средний балл, посещаемость и рекомендации", "[services]") {
    Student student = makeStudent("S-1");
    Teacher teacher = makeTeacher();
    StudentGroup group = makeGroup("IS-21");
    group.addStudent(student);
    Discipline discipline = makeDiscipline();
    Semester semester = makeSemester();
    Course course("C-1", &discipline, &teacher, &semester, &group, 72, nullptr, "active");
    Lesson lesson1("L-1", &course, "2026-09-01", "10:00", "11:30", "lecture", nullptr, "Тема 1");
    Lesson lesson2("L-2", &course, "2026-09-08", "10:00", "11:30", "lecture", nullptr, "Тема 2");
    Lesson lesson3("L-3", &course, "2026-09-15", "10:00", "11:30", "lecture", nullptr, "Тема 3");
    Lesson lesson4("L-4", &course, "2026-09-22", "10:00", "11:30", "lecture", nullptr, "Тема 4");

    SECTION("оценки 80 и 90, посещено 3 из 4") {
        GradeBook book(&student);
        std::vector<Grade> grades;
        grades.reserve(2);
        grades.emplace_back("G-1", &student, &discipline, &teacher, 80, "2026-10-01", "exam",
                            &semester);
        grades.emplace_back("G-2", &student, &discipline, &teacher, 90, "2026-10-02", "credit",
                            &semester);
        book.addGrade(grades[0]);
        book.addGrade(grades[1]);

        AttendanceJournal journal(&group, &course);
        std::vector<Attendance> entries;
        entries.reserve(4);
        entries.emplace_back("A-1", &lesson1, &student, "present");
        entries.emplace_back("A-2", &lesson2, &student, "present");
        entries.emplace_back("A-3", &lesson3, &student, "late", 5);
        entries.emplace_back("A-4", &lesson4, &student, "absent");
        for (Attendance& entry : entries) journal.mark(entry);

        ProgressReport report("PR-1", &student, &semester);
        report.buildFrom(book, journal, "2026-12-25");
        REQUIRE(report.getAverageScore() == Catch::Approx(85.0));
        REQUIRE(report.getDebtsCount() == 0);
        REQUIRE(report.getAttendancePercent() == Catch::Approx(75.0));
        REQUIRE(report.getGeneratedAt() == "2026-12-25");
        REQUIRE_FALSE(report.hasDebts());
        REQUIRE(report.isSuccessful());
        REQUIRE_FALSE(report.needsImprovement());
        REQUIRE(report.getRecommendations() != "нет замечаний");
        REQUIRE(report.getRecommendations() == "требуется улучшение успеваемости");
        REQUIRE(report.summaryText().find("85") != std::string::npos);
        REQUIRE(report.summaryText().find("75") != std::string::npos);
    }

    SECTION("нет замечаний только при 85+, без долгов и 90+ посещаемости") {
        GradeBook book(&student);
        std::vector<Grade> grades;
        grades.reserve(2);
        grades.emplace_back("G-3", &student, &discipline, &teacher, 90, "2026-10-01", "exam",
                            &semester);
        grades.emplace_back("G-4", &student, &discipline, &teacher, 95, "2026-10-02", "credit",
                            &semester);
        book.addGrade(grades[0]);
        book.addGrade(grades[1]);

        AttendanceJournal journal(&group, &course);
        std::vector<Attendance> entries;
        entries.reserve(4);
        entries.emplace_back("A-5", &lesson1, &student, "present");
        entries.emplace_back("A-6", &lesson2, &student, "present");
        entries.emplace_back("A-7", &lesson3, &student, "present");
        entries.emplace_back("A-8", &lesson4, &student, "present");
        for (Attendance& entry : entries) journal.mark(entry);

        ProgressReport report("PR-2", &student, &semester);
        report.buildFrom(book, journal, "2026-12-25");
        REQUIRE(report.getAverageScore() == Catch::Approx(92.5));
        REQUIRE(report.getAttendancePercent() == Catch::Approx(100.0));
        REQUIRE(report.getRecommendations() == "нет замечаний");
        report.setRatingPlace(1);
        REQUIRE(report.getRatingPlace() == 1);
    }

    SECTION("задолженность ухудшает отчёт") {
        GradeBook book(&student);
        std::vector<Grade> grades;
        grades.reserve(2);
        grades.emplace_back("G-5", &student, &discipline, &teacher, 40, "2026-10-01", "exam",
                            &semester);
        grades.emplace_back("G-6", &student, &discipline, &teacher, 80, "2026-10-02", "credit",
                            &semester);
        book.addGrade(grades[0]);
        book.addGrade(grades[1]);

        AttendanceJournal journal(&group, &course);
        std::vector<Attendance> entries;
        entries.reserve(2);
        entries.emplace_back("A-9", &lesson1, &student, "absent");
        entries.emplace_back("A-10", &lesson2, &student, "present");
        for (Attendance& entry : entries) journal.mark(entry);

        ProgressReport report("PR-3", &student, &semester);
        report.buildFrom(book, journal, "2026-12-25");
        REQUIRE(report.getAverageScore() == Catch::Approx(60.0));
        REQUIRE(report.getDebtsCount() == 1);
        REQUIRE(report.hasDebts());
        REQUIRE_FALSE(report.isSuccessful());
        REQUIRE(report.needsImprovement());
        REQUIRE(report.getRecommendations() == "требуется улучшение успеваемости");
    }

    SECTION("сравнение отчётов") {
        ProgressReport best("PR-4", &student, &semester);
        ProgressReport good("PR-5", &student, &semester);
        GradeBook highBook(&student);
        std::vector<Grade> highGrades;
        highGrades.reserve(2);
        highGrades.emplace_back("G-7", &student, &discipline, &teacher, 90, "2026-10-01", "exam",
                                &semester);
        highGrades.emplace_back("G-8", &student, &discipline, &teacher, 90, "2026-10-02", "credit",
                                &semester);
        highBook.addGrade(highGrades[0]);
        highBook.addGrade(highGrades[1]);
        AttendanceJournal fullJournal(&group, &course);
        std::vector<Attendance> fullEntries;
        fullEntries.reserve(2);
        fullEntries.emplace_back("A-11", &lesson1, &student, "present");
        fullEntries.emplace_back("A-12", &lesson2, &student, "present");
        for (Attendance& entry : fullEntries) fullJournal.mark(entry);
        best.buildFrom(highBook, fullJournal, "2026-12-25");

        GradeBook lowBook(&student);
        std::vector<Grade> lowGrades;
        lowGrades.reserve(2);
        lowGrades.emplace_back("G-9", &student, &discipline, &teacher, 70, "2026-10-01", "exam",
                               &semester);
        lowGrades.emplace_back("G-10", &student, &discipline, &teacher, 80, "2026-10-02", "credit",
                               &semester);
        lowBook.addGrade(lowGrades[0]);
        lowBook.addGrade(lowGrades[1]);
        AttendanceJournal lowJournal(&group, &course);
        std::vector<Attendance> lowEntries;
        lowEntries.reserve(2);
        lowEntries.emplace_back("A-13", &lesson1, &student, "present");
        lowEntries.emplace_back("A-14", &lesson2, &student, "absent");
        for (Attendance& entry : lowEntries) lowJournal.mark(entry);
        good.buildFrom(lowBook, lowJournal, "2026-12-25");

        REQUIRE(best.compareWith(good));
        REQUIRE_FALSE(good.compareWith(best));
        REQUIRE(good.getAverageScore() == Catch::Approx(75.0));
    }

    SECTION("чужая зачётная книжка и некорректная дата") {
        Student other = makeStudent("S-2");
        GradeBook foreign(&other);
        AttendanceJournal journal(&group, &course);
        ProgressReport report("PR-6", &student, &semester);
        REQUIRE_THROWS_AS(report.buildFrom(foreign, journal, "2026-12-25"),
                          InvalidStudentDataException);
        GradeBook own(&student);
        REQUIRE_THROWS_AS(report.buildFrom(own, journal, "25.12.2026"),
                          InvalidStudentDataException);
        REQUIRE_THROWS_AS(ProgressReport("", &student, &semester), InvalidStudentDataException);
        REQUIRE_THROWS_AS(ProgressReport("PR-7", nullptr, &semester), InvalidStudentDataException);
    }
}

TEST_CASE("StudentDirectory: поиск, фильтры и рейтинг", "[services]") {
    Teacher teacher = makeTeacher();
    Discipline discipline = makeDiscipline();
    Student first = makeStudent("S-1", "male", "active", "Иванов", 1);
    Student second = makeStudent("S-2", "female", "active", "Петров", 2);
    Student third = makeStudent("S-3", "male", "active", "Сидоров", 1);
    Student inactive = makeStudent("S-4", "male", "expelled", "Морозов", 1);
    StudentGroup groupA = makeGroup("IS-21");
    StudentGroup groupB = makeGroup("IS-22");
    groupA.addStudent(first);
    groupA.addStudent(second);
    groupB.addStudent(third);
    groupB.addStudent(inactive);

    Grade firstHigh("G-1", &first, &discipline, &teacher, 80, "2026-10-01", "exam");
    Grade firstExcellent("G-2", &first, &discipline, &teacher, 90, "2026-10-02", "credit");
    Grade secondFail("G-3", &second, &discipline, &teacher, 40, "2026-10-03", "exam");
    Grade inactiveHigh("G-4", &inactive, &discipline, &teacher, 100, "2026-10-04", "exam");
    first.recordGrade(firstHigh);
    first.recordGrade(firstExcellent);
    second.recordGrade(secondFail);
    inactive.recordGrade(inactiveHigh);

    StudentDirectory directory;
    SECTION("добавление без дублей") {
        directory.addStudent(first);
        directory.addStudent(second);
        directory.addStudent(third);
        directory.addStudent(inactive);
        directory.addStudent(first);
        directory.addGroup(groupA);
        directory.addGroup(groupB);
        directory.addGroup(groupA);
        directory.addTeacher(teacher);
        directory.addTeacher(teacher);
        REQUIRE(directory.getStudents().size() == 4);
        REQUIRE(directory.getGroups().size() == 2);
        REQUIRE(directory.getTeachers().size() == 1);
    }

    SECTION("поиск по номеру и ФИО") {
        directory.addStudent(first);
        directory.addStudent(second);
        REQUIRE(directory.findById("S-1") == &first);
        REQUIRE(directory.findById("P-S-1") == &first);
        REQUIRE(directory.findById("S-404") == nullptr);
        REQUIRE(directory.findByName("петров") == &second);
        REQUIRE(directory.findByName("ПЕТРОВ ИВАН") == &second);
        REQUIRE(directory.findByName("неизвестный") == nullptr);
    }

    SECTION("фильтры по группе и курсу") {
        directory.addStudent(first);
        directory.addStudent(second);
        directory.addStudent(third);
        directory.addStudent(inactive);
        directory.addGroup(groupA);
        directory.addGroup(groupB);
        REQUIRE(directory.filterByGroup("IS-21").size() == 2);
        REQUIRE(directory.filterByGroup("IS-22").size() == 2);
        REQUIRE(directory.filterByGroup("IS-404").empty());
        REQUIRE(directory.filterByCourse(1).size() == 4);
        REQUIRE(directory.filterByCourse(2).empty());
        REQUIRE(directory.filterByCourse(5).empty());
    }

    SECTION("рейтинг и задолженности") {
        directory.addStudent(first);
        directory.addStudent(second);
        directory.addStudent(third);
        directory.addStudent(inactive);
        std::vector<Student*> top = directory.topStudents(2);
        REQUIRE(top.size() == 2);
        REQUIRE(top[0] == &first);
        REQUIRE(top[1] == &second);
        REQUIRE(directory.topStudents(10).size() == 3);
        REQUIRE(directory.topStudents(0).empty());
        REQUIRE(directory.topStudents(-1).empty());

        std::vector<Student*> debts = directory.studentsWithDebt();
        REQUIRE(debts.size() == 1);
        REQUIRE(debts[0] == &second);
    }
}

TEST_CASE("StudentDirectory: CSV-выгрузка с экранированием", "[services]") {
    Teacher teacher = makeTeacher();
    Discipline discipline = makeDiscipline();
    Student student = makeStudent("S-1", "male", "active", "Петров, \"мл.\"", 2, "contract");
    StudentGroup group = makeGroup("IS-21");
    group.addStudent(student);
    Grade grade("G-1", &student, &discipline, &teacher, 85, "2026-10-01", "exam");
    student.recordGrade(grade);

    StudentDirectory directory;
    directory.addStudent(student);
    const std::string csv = directory.exportCsv();
    REQUIRE(csv.rfind("studentId,fullName,group,course,status,funding,average", 0) == 0);
    REQUIRE(csv.find("S-1") != std::string::npos);
    REQUIRE(csv.find("IS-21") != std::string::npos);
    REQUIRE(csv.find("\"Петров, \"\"мл.\"\" Иван Иванович\"") != std::string::npos);
    REQUIRE(csv.find("85.00") != std::string::npos);

    StudentDirectory empty;
    REQUIRE(empty.exportCsv() == "studentId,fullName,group,course,status,funding,average\n");
}
