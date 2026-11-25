# README — лабораторная работа №3. Информационная система студентов

Предметная область: **информационная система студентов** — университет, учебные
подразделения, образовательные программы, дисциплины, расписание, успеваемость,
финансы, документы и студенческие сервисы.

## Структура проекта

```
StudentInfo/
├── include/studentis/
│   ├── core/         — университет, люди, группы, учебные планы (12 классов)
│   ├── academic/     — дисциплины, занятия, расписание, задания (10 классов)
│   ├── assessment/   — оценки, экзамены, посещаемость, ведомости (8 классов)
│   ├── finance/      — обучение, платежи, стипендии, задолженности (6 классов)
│   ├── documents/    — справки, приказы, дипломы, переводы (8 классов)
│   ├── services/     — библиотека, общежитие, уведомления, мероприятия (10 классов)
│   └── exceptions/   — иерархия исключений (16 классов)
├── src/              — реализация классов и демонстрационное приложение
└── tests/            — модульные тесты Catch2
```

## Формат таблицы

`Класс | число полей | число содержательных методов | связанные классы`

### core/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| University | 6 | 8 | Teacher, Faculty, Student, AcademicYear, StudentGroup |
| Person | 9 | 7 | — |
| Student | 9 | 13 | StudentGroup, Grade, Exam, AcademicLeave |
| Teacher | 8 | 9 | Department, Discipline, Thesis, Grade |
| StudentGroup | 8 | 10 | Specialty, Teacher, Student |
| Faculty | 7 | 7 | Teacher, Department, Specialty, Student |
| Department | 7 | 7 | Faculty, Teacher, Discipline |
| Specialty | 7 | 7 | Faculty, Curriculum |
| Curriculum | 6 | 8 | Specialty, StudyPlanEntry, Discipline |
| StudyPlanEntry | 8 | 7 | Discipline |
| AcademicYear | 5 | 6 | Semester |
| Semester | 7 | 7 | AcademicYear |

### academic/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Discipline | 7 | 7 | Department, Student |
| Course | 9 | 8 | Discipline, Teacher, Semester, StudentGroup, Classroom, Lesson |
| Lesson | 9 | 8 | Course, Classroom, Lesson |
| Classroom | 8 | 6 | StudentGroup, Lesson |
| Schedule | 5 | 8 | StudentGroup, Semester, Lesson |
| ScheduleChange | 9 | 6 | Lesson, Classroom, Teacher, StudentGroup |
| TeachingLoad | 5 | 6 | Teacher, Semester, Course |
| CourseMaterial | 8 | 6 | Course |
| Assignment | 9 | 8 | Course, Submission, Student |
| Submission | 9 | 7 | Assignment, Student, Teacher |

### assessment/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Grade | 9 | 7 | Student, Discipline, Teacher, Semester |
| Exam | 10 | 8 | Discipline, StudentGroup, Teacher, Classroom, ExamTicket, Student |
| ExamTicket | 6 | 5 | Exam |
| ExamResult | 7 | 6 | Exam, Student |
| Attendance | 7 | 6 | Lesson, Student, Teacher |
| AttendanceJournal | 3 | 6 | StudentGroup, Course, Attendance, Lesson, Student, Teacher |
| GradeBook | 3 | 8 | Student, Grade, ExamResult, Semester, Discipline |
| Retake | 8 | 6 | ExamResult, Student, Teacher |

### finance/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| TuitionContract | 9 | 8 | Student, Specialty |
| TuitionPayment | 8 | 7 | TuitionContract, Teacher |
| Scholarship | 8 | 6 | Student, GradeBook, AttendanceJournal |
| ScholarshipAward | 8 | 6 | Scholarship, Student, Semester |
| StudentFine | 8 | 6 | Student |
| FinanceAccount | 3 | 7 | Student, TuitionPayment, ScholarshipAward |

### documents/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| StudentCard | 7 | 7 | Student |
| EnrollmentOrder | 8 | 6 | Student, StudentGroup, Teacher |
| AcademicLeave | 8 | 6 | Student, Teacher, AcademicLeave |
| Certificate | 9 | 6 | Student, Teacher |
| Diploma | 10 | 7 | Student, Specialty |
| Thesis | 9 | 7 | Student, Teacher |
| PracticeDiary | 9 | 7 | Student, Teacher |
| TransferApplication | 9 | 6 | Student, StudentGroup, Teacher |

### services/

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| LibraryCard | 6 | 7 | Student, BookLoan |
| Book | 8 | 7 | — |
| BookLoan | 8 | 6 | Book, Student |
| Dormitory | 7 | 7 | Teacher, DormRoom, Student |
| DormRoom | 7 | 7 | Dormitory, Student |
| Notification | 9 | 7 | Student |
| NotificationService | 3 | 7 | Notification, Student, StudentGroup |
| Event | 11 | 9 | Teacher, Classroom, Student, Event |
| ProgressReport | 9 | 6 | Student, Semester, GradeBook, AttendanceJournal, ProgressReport |
| StudentDirectory | 3 | 10 | Student, StudentGroup, Teacher |

`register` — зарезервированное слово C++, поэтому одноимённые по смыслу
поведения реализованы как `Diploma::registerIssue` и
`Event::registerStudent`; остальные публичные имена совпадают с заданием.

## Исключения (16)

Базовый класс `StudentInfoException` наследует `std::runtime_error`. Остальные
исключения наследуют его и позволяют перехватывать все ошибки предметной
области одним `catch`.

- `InvalidStudentDataException` — пустые обязательные данные, неверная дата,
  статус или контакты;
- `StudentNotFoundException` — студент не найден;
- `GroupNotFoundException` — группа не найдена;
- `GroupFullException` — группа заполнена;
- `DisciplineNotFoundException` — дисциплина не найдена;
- `PrerequisiteNotMetException` — не выполнены предпосылки дисциплины;
- `ScheduleConflictException` — конфликт расписания;
- `ClassroomCapacityException` — не хватает мест или оборудования;
- `InvalidGradeException` — недопустимая оценка;
- `ExamRegistrationException` — недопустимая регистрация на экзамен;
- `AttendanceException` — недопустимая отметка посещаемости;
- `InsufficientFundsException` — недопустимая денежная операция или переплата;
- `PaymentNotFoundException` — платёж не найден;
- `DocumentExpiredException` — документ просрочен или заблокирован;
- `DormitoryFullException` — нет мест или нарушено правило заселения.

## Сборка и проверка

Из корня репозитория:

```bash
cmake -S Lab_3/StudentInfo -B Lab_3/StudentInfo/build -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=ON
cmake --build Lab_3/StudentInfo/build
ctest --test-dir Lab_3/StudentInfo/build --output-on-failure
cmake --build Lab_3/StudentInfo/build --target coverage
./Lab_3/StudentInfo/build/studentinfo_app
```

## Итоговая статистика

| Показатель | Значение |
|---|---|
| Классов | 70 (54 доменных + 16 исключений) |
| Полей | 402 (399 по инвентарю + 3 приватных вспомогательных контейнера) |
| Содержательных поведений | 383 |
| Ассоциаций | 127 уникальных пар (144 направленных) |
| Исключений | 16 |

Приватные контейнеры `Course::lessons_`, `Classroom::lessons_` и
`Exam::registered_` нужны для проверки расписания и повторной регистрации; в
публичную объектную модель они не входят.

Требования задания выполнены: не менее 50 классов, 150 полей, 100 поведений,
30 ассоциаций и 12 исключений.
