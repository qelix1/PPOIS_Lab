# AGENTS.md — Lab 3: «Информационная система студентов»

## 1. Task

Implement Lab 3 in `Lab_3/StudentInfo/` as a C++17 object model for **«Информационная система студентов»**. Read the assignment in `Lab_3/lab3 (1).md`.

Hard requirements from the assignment:

- at least 50 classes;
- at least 150 fields in total;
- at least 100 meaningful domain behaviors (not getters/setters);
- at least 30 associations (a class used as another class's field or method parameter);
- at least 12 custom exception classes;
- `Lab_3/README.md` documents every class, its field count, behavior count, associations, and all exceptions.

This specification defines **54 domain classes + 16 exception classes = 70 classes**, **399 fields**, and **383 behaviors**. Implement the inventory below without renaming classes, fields, or methods. If a helper is needed, add it privately; do not replace the public model.

## 2. Conventions copied from Lab 2

Use `Lab_2/AutoService` on branch `Lab_2` as the style reference. On the `Lab_3` branch its files are not checked out; inspect them with, for example:

```bash
git show Lab_2:Lab_2/AutoService/CMakeLists.txt
git show Lab_2:Lab_2/AutoService/include/autoservice/core/Client.h
git show Lab_2:Lab_2/AutoService/src/core/Client.cpp
git show Lab_2:Lab_2/AutoService/tests/test_core.cpp
```

Rules:

- C++17, CMake >= 3.14, namespace `studentis`.
- One header per class: `include/studentis/<module>/<Class>.h`.
- One implementation file per class: `src/<module>/<Class>.cpp`.
- Header guards: `STUDENTIS_<MODULE>_<CLASS>_H`.
- Doxygen comments in Russian for classes and public behaviors, matching Lab 2 tone.
- Getters/setters are inline in headers and do **not** count as behaviors.
- Associations are non-owning raw pointers (`Class*`) and `std::vector<Class*>`; forward-declare in headers, include the full header in `.cpp`. Never delete pointed-to objects.
- IDs, names, statuses, and dates are `std::string`. Dates are ISO `YYYY-MM-DD`; date-times are `YYYY-MM-DD HH:MM`.
- Money, averages, percentages, and rates are `double`; counts, hours, scores, years, and capacities are `int`; flags are `bool`.
- Use `std::move` for string constructor parameters where appropriate. No external runtime dependencies.
- Validate at public behavior boundaries and throw the mapped domain exception. Return `bool` only where the contract below says the outcome is a normal query/registration result.
- After completing the implementation and passing all acceptance checks, commit the intended changes with a clear commit message; do not push unless explicitly requested.
- Reuse the Lab 2 date helper as `include/studentis/core/DateUtils.h`, changing only the include guard and namespace to `studentis`:

```bash
mkdir -p Lab_3/StudentInfo/include/studentis/core
git show Lab_2:Lab_2/AutoService/include/autoservice/core/DateUtils.h > Lab_3/StudentInfo/include/studentis/core/DateUtils.h
# Then rename AUTOSERVICE_* guard to STUDENTIS_CORE_DATE_UTILS_H and namespace autoservice to studentis.
```

## 3. Project skeleton and build

Create:

```text
Lab_3/StudentInfo/
├── CMakeLists.txt
├── Doxyfile
├── cmake/run_coverage.cmake
├── include/studentis/
│   ├── core/
│   ├── academic/
│   ├── assessment/
│   ├── finance/
│   ├── documents/
│   ├── services/
│   └── exceptions/
├── src/
│   ├── core/
│   ├── academic/
│   ├── assessment/
│   ├── finance/
│   ├── documents/
│   ├── services/
│   └── main.cpp
└── tests/
    ├── test_core.cpp
    ├── test_academic.cpp
    ├── test_assessment.cpp
    ├── test_finance.cpp
    ├── test_documents.cpp
    └── test_services.cpp
```

`CMakeLists.txt` must mirror Lab 2 with these exact substitutions:

- `project(student_info CXX)`;
- library `studentinfo_lib` from `file(GLOB_RECURSE STUDENTINFO_SOURCES CONFIGURE_DEPENDS src/*.cpp)` with `src/main.cpp` removed;
- executable `studentinfo_app` from `src/main.cpp`, linked to `studentinfo_lib`;
- public include directory `include`;
- option `ENABLE_COVERAGE` default `OFF`, using PUBLIC `--coverage -O0 -g` compile/link flags on `studentinfo_lib` when enabled;
- Catch2 via `FetchContent`, GIT_TAG `v3.5.2`;
- `studentinfo_tests` from all six `tests/test_*.cpp` files, linked to `Catch2::Catch2WithMain studentinfo_lib`;
- `add_test(NAME studentinfo_tests COMMAND studentinfo_tests)`;
- `install(TARGETS studentinfo_app DESTINATION bin)`;
- optional `coverage` target calling `cmake/run_coverage.cmake`, same variable contract as Lab 2 (`LCOV`, `GENHTML`, `TESTS`, `BUILD_DIR`, `SRC_DIR`).

`cmake/run_coverage.cmake` must: delete old `*.gcda`, run `${TESTS}`, capture with `lcov`, extract every `${SRC_DIR}/src/*.cpp` **except `src/main.cpp`**, print the summary, and generate `build/coverage_report/index.html`. Do not copy Lab 2's stale hard-coded `PostMachine.cpp`/`Tape.cpp` extraction list.

`Doxyfile`: copy Lab 2's Doxyfile, set `PROJECT_NAME = "PPOIS Lab 3 - Student Information System"`, keep `INPUT = include src`, `RECURSIVE = YES`, `EXTRACT_ALL = YES`, `GENERATE_LATEX = NO`, `OUTPUT_DIRECTORY = doc`.

## 4. Implementation order

Build and test after each numbered step; do not write all files before the first compile.

1. Skeleton, CMake, `DateUtils`, and all 16 exception headers.
2. `core` classes and `tests/test_core.cpp`.
3. `academic` classes and `tests/test_academic.cpp`.
4. `assessment` classes and `tests/test_assessment.cpp`.
5. `finance` classes and `tests/test_finance.cpp`.
6. `documents` classes and `tests/test_documents.cpp`.
7. `services` classes and `tests/test_services.cpp`.
8. `src/main.cpp` demo, Doxygen, coverage.
9. `Lab_3/README.md` with the final class tables and measured totals.

Notation in the inventory: `Class*` is a non-owning pointer field; `Class[]` is `std::vector<Class*>`; a leading number is the field or behavior count. Inherited fields are not repeated in `Student`/`Teacher`.

## 5. Domain inventory — core (12 classes, 87 fields, 96 behaviors)

|Class|Fields|Behaviors|Associations|
|---|---|---|---|
|University|6: name_, rector_ Teacher*, foundedYear_, faculties_ Faculty[], students_ Student[], currentYear_ AcademicYear*|8: addFaculty, findFaculty, enrollStudent, expelStudent, totalStudents, findGroup, setCurrentYear, summary|Teacher, Faculty, Student, AcademicYear, StudentGroup|
|Person|9: id_, lastName_, firstName_, middleName_, birthDate_, gender_, phone_, email_, address_|7: fullName, shortName, ageOn, isAdultOn, updateContacts, matchesName, changeAddress|—|
|Student : Person|9 own: studentId_, group_ StudentGroup*, enrolledAt_, status_, educationForm_, funding_, courseNumber_, grades_ Grade[], notes_|13: assignGroup, transferTo, expel, restore, advanceCourse, isActive, isBudget, recordGrade, gradesForDiscipline, averageGrade, hasAcademicDebt, canTakeExam, requestAcademicLeave|StudentGroup, Grade, Exam, AcademicLeave|
|Teacher : Person|8 own: teacherId_, department_ Department*, position_, academicDegree_, academicTitle_, hiredAt_, baseWorkloadHours_, disciplines_ Discipline[]|9: assignDiscipline, removeDiscipline, canTeach, plannedWorkload, isProfessor, yearsOfService, superviseThesis, markGrade, workloadPercent|Department, Discipline, Thesis, Grade|
|StudentGroup|8: code_, specialty_ Specialty*, admissionYear_, courseNumber_, curator_ Teacher*, maxSize_, students_ Student[], status_|10: addStudent, removeStudent, findStudent, activeCount, isFull, averageScore, studentsWithDebts, advanceCourse, assignCurator, availableSeats|Specialty, Teacher, Student|
|Faculty|7: id_, name_, shortName_, dean_ Teacher*, phone_, building_, departments_ Department[]|7: addDepartment, findDepartment, allSpecialties, appointDean, studentCount, teacherCount, rename|Teacher, Department, Specialty, Student|
|Department|7: id_, name_, faculty_ Faculty*, head_ Teacher*, phone_, teachers_ Teacher[], disciplines_ Discipline[]|7: addTeacher, removeTeacher, assignDiscipline, findDiscipline, teachersWithDegree, totalPlannedWorkload, appointHead|Faculty, Teacher, Discipline|
|Specialty|7: code_, name_, faculty_ Faculty*, qualification_, studyYears_, tuitionFee_, budgetSeats_|7: isLongProgram, annualFee, hasBudgetSeats, durationMonths, matchesCode, createCurriculum, rename|Faculty, Curriculum|
|Curriculum|6: id_, specialty_ Specialty*, admissionYear_, approvedAt_, totalCredits_, entries_ StudyPlanEntry[]|8: addEntry, removeEntry, creditsForSemester, requiredDisciplines, electiveDisciplines, findEntry, validate, recalcCredits|Specialty, StudyPlanEntry, Discipline|
|StudyPlanEntry|8: discipline_ Discipline*, semesterNumber_, credits_, lectureHours_, practiceHours_, labHours_, controlForm_, mandatory_|7: totalHours, isExam, isCredit, isElective, matchesSemester, validateHours, changeControlForm|Discipline|
|AcademicYear|5: name_, startDate_, endDate_, semesters_ Semester[], current_|6: addSemester, containsDate, findSemester, isCurrent, durationDays, close|Semester|
|Semester|7: number_, year_ AcademicYear*, startDate_, endDate_, examsStart_, examsEnd_, status_|7: containsDate, isExamPeriod, weeksCount, canEditGrades, close, isActive, daysUntilExams|AcademicYear|

## 6. Domain inventory — academic (10 classes, 76 fields, 70 behaviors)

|Class|Fields|Behaviors|Associations|
|---|---|---|---|
|Discipline|7: id_, name_, code_, department_ Department*, credits_, description_, prerequisites_ Discipline[]|7: addPrerequisite, hasPrerequisite, canStudy, matchesName, assignDepartment, totalPrerequisites, summary|Department, Student|
|Course|8: id_, discipline_ Discipline*, teacher_ Teacher*, semester_ Semester*, group_ StudentGroup*, plannedHours_, classroom_ Classroom*, status_|8: assignTeacher, assignClassroom, addLesson, completionPercent, isFinished, studentsCount, changeStatus, matchesGroup|Discipline, Teacher, Semester, StudentGroup, Classroom, Lesson|
|Lesson|9: id_, course_ Course*, date_, startTime_, endTime_, type_, classroom_ Classroom*, topic_, conducted_|8: durationMinutes, overlaps, moveTo, assignClassroom, markConducted, isLecture, isOn, summary|Course, Classroom, Lesson|
|Classroom|7: id_, building_, roomNumber_, capacity_, type_, hasProjector_, hasComputers_|6: fits, supports, isAvailable, utilizationPercent, matchesType, rename|StudentGroup, Lesson|
|Schedule|5: id_, group_ StudentGroup*, semester_ Semester*, lessons_ Lesson[], published_|8: addLesson, removeLesson, lessonsOn, lessonsForWeekday, validateNoConflicts, publish, findFreeSlot, totalWeeklyHours|StudentGroup, Semester, Lesson|
|ScheduleChange|9: id_, lesson_ Lesson*, oldDate_, newDate_, oldClassroom_ Classroom*, newClassroom_ Classroom*, reason_, approvedBy_ Teacher*, createdAt_|6: approve, apply, cancel, isApplied, affectsGroup, summary|Lesson, Classroom, Teacher, StudentGroup|
|TeachingLoad|5: teacher_ Teacher*, semester_ Semester*, plannedHours_, conductedHours_, courses_ Course[]|6: addCourse, registerConductedHours, remainingHours, overloadHours, completionPercent, isOverloaded|Teacher, Semester, Course|
|CourseMaterial|8: id_, course_ Course*, title_, type_, url_, uploadedAt_, sizeKb_, mandatory_|6: isLink, isMandatory, matchesType, rename, olderThan, summary|Course|
|Assignment|9: id_, course_ Course*, title_, description_, issuedAt_, deadline_, maxScore_, type_, submissions_ Submission[]|8: addSubmission, submissionFor, isOverdue, gradedCount, averageScore, completionPercent, extendDeadline, hasSubmission|Course, Submission, Student|
|Submission|9: id_, assignment_ Assignment*, student_ Student*, submittedAt_, content_, score_, status_, feedback_, checkedBy_ Teacher*|7: submit, grade, isLate, needsRevision, returnForRevision, isGraded, summary|Assignment, Student, Teacher|

## 7. Domain inventory — assessment (8 classes, 52 fields, 52 behaviors)

|Class|Fields|Behaviors|Associations|
|---|---|---|---|
|Grade|9: id_, student_ Student*, discipline_ Discipline*, teacher_ Teacher*, value_, date_, controlForm_, semester_ Semester*, comment_|7: isPass, isExcellent, isFail, updateValue, matchesSemester, weight, summary|Student, Discipline, Teacher, Semester|
|Exam|9: id_, discipline_ Discipline*, group_ StudentGroup*, teacher_ Teacher*, date_, classroom_ Classroom*, maxScore_, tickets_ ExamTicket[], status_|8: addTicket, removeTicket, isScheduledOn, canStart, registerStudent, registeredCount, complete, summary|Discipline, StudentGroup, Teacher, Classroom, ExamTicket, Student|
|ExamTicket|6: number_, exam_ Exam*, question1_, question2_, question3_, difficulty_|5: questionCount, isHard, matchesNumber, changeDifficulty, summary|Exam|
|ExamResult|7: exam_ Exam*, student_ Student*, score_, grade_, answers_, checkedAt_, retakeAllowed_|6: isPassed, isExcellent, toFiveScale, needsRetake, allowRetake, summary|Exam, Student|
|Attendance|7: id_, lesson_ Lesson*, student_ Student*, status_, minutesLate_, reason_, markedBy_ Teacher*|6: isPresent, isAbsent, isExcused, markExcused, latePenalty, summary|Lesson, Student, Teacher|
|AttendanceJournal|3: group_ StudentGroup*, course_ Course*, entries_ Attendance[]|6: mark, attendanceRate, absentCount, unexcusedAbsences, chronicAbsentees, lessonRate|StudentGroup, Course, Attendance, Lesson, Student, Teacher|
|GradeBook|3: student_ Student*, grades_ Grade[], results_ ExamResult[]|8: addGrade, addExamResult, average, averageForSemester, hasDebts, debtDisciplines, excellentCount, ratingScore|Student, Grade, ExamResult, Semester, Discipline|
|Retake|8: id_, result_ ExamResult*, student_ Student*, scheduledDate_, teacher_ Teacher*, reason_, status_, newScore_|6: schedule, complete, isPending, canRetake, isSuccessful, cancel|ExamResult, Student, Teacher|

## 8. Domain inventory — finance (6 classes, 44 fields, 40 behaviors)

|Class|Fields|Behaviors|Associations|
|---|---|---|---|
|TuitionContract|9: id_, student_ Student*, specialty_ Specialty*, signedAt_, startDate_, endDate_, annualAmount_, discountPercent_, status_|8: yearlyTotal, semesterAmount, isActive, applyDiscount, terminate, daysUntilExpiry, remainingAmount, validateDates|Student, Specialty|
|TuitionPayment|8: id_, contract_ TuitionContract*, amount_, paidAt_, method_, receiptNumber_, confirmedBy_ Teacher*, status_|7: confirm, cancel, isConfirmed, isLate, matchesPeriod, receipt, summary|TuitionContract, Teacher|
|Scholarship|8: id_, name_, type_, amount_, minAverage_, maxAbsences_, budgetLimit_, active_|6: isEligible, amountFor, isSocial, isAcademic, requireAverage, deactivate|Student, GradeBook, AttendanceJournal|
|ScholarshipAward|8: id_, scholarship_ Scholarship*, student_ Student*, semester_ Semester*, amount_, awardedAt_, status_, orderNumber_|6: approve, pay, cancel, isPaid, matchesSemester, summary|Scholarship, Student, Semester|
|StudentFine|8: id_, student_ Student*, reason_, amount_, issuedAt_, dueDate_, paidAmount_, status_|6: addPayment, remaining, isOverdue, isPaid, waive, summary|Student|
|FinanceAccount|3: student_ Student*, balance_, transactions_ TuitionPayment[]|7: charge, registerPayment, canPay, debt, historyBetween, hasDebt, applyScholarship|Student, TuitionPayment, ScholarshipAward|

## 9. Domain inventory — documents (8 classes, 69 fields, 52 behaviors)

|Class|Fields|Behaviors|Associations|
|---|---|---|---|
|StudentCard|7: number_, student_ Student*, issuedAt_, expiresAt_, status_, pinHash_, accessLevel_|7: activate, block, renew, isValid, verifyPin, canEnter, daysUntilExpiry|Student|
|EnrollmentOrder|8: id_, student_ Student*, group_ StudentGroup*, orderDate_, orderNumber_, basis_, signedBy_ Teacher*, status_|6: sign, execute, cancel, isExecuted, isBudget, summary|Student, StudentGroup, Teacher|
|AcademicLeave|8: id_, student_ Student*, startDate_, endDate_, reason_, orderNumber_, status_, approvedBy_ Teacher*|6: approve, extend, close, isActive, durationDays, overlaps|Student, Teacher, AcademicLeave|
|Certificate|9: id_, student_ Student*, type_, issuedAt_, issuedBy_ Teacher*, purpose_, serialNumber_, validUntil_, text_|6: isValid, generateText, isForMilitary, daysUntilExpiry, revoke, summary|Student, Teacher|
|Diploma|10: id_, student_ Student*, specialty_ Specialty*, number_, series_, issueDate_, finalAverage_, honors_, thesisTopic_, thesisScore_|7: isHonors, qualification, isIssued, register, summary, validateNumber, matchesSpecialty|Student, Specialty|
|Thesis|9: id_, student_ Student*, supervisor_ Teacher*, topic_, submittedAt_, defenseDate_, plagiarismPercent_, score_, status_|7: submit, checkPlagiarism, assignReviewer, isAllowedToDefend, grade, isPassed, daysToDefense|Student, Teacher|
|PracticeDiary|9: id_, student_ Student*, supervisor_ Teacher*, organization_, startDate_, endDate_, hours_, tasks_ std::vector<std::string>, finalScore_|7: addTask, totalHours, isComplete, rate, durationDays, summary, validateDates|Student, Teacher|
|TransferApplication|9: id_, student_ Student*, fromGroup_ StudentGroup*, toGroup_ StudentGroup*, reason_, createdAt_, status_, decidedBy_ Teacher*, decisionDate_|6: approve, reject, isPending, isValid, apply, summary|Student, StudentGroup, Teacher|

## 10. Domain inventory — services (10 classes, 71 fields, 73 behaviors)

|Class|Fields|Behaviors|Associations|
|---|---|---|---|
|LibraryCard|6: id_, student_ Student*, issuedAt_, expiresAt_, status_, loans_ BookLoan[]|7: addLoan, returnLoan, hasOverdue, activeLoans, isValid, renew, debtFine|Student, BookLoan|
|Book|8: id_, title_, author_, isbn_, year_, copiesTotal_, copiesAvailable_, category_|7: isAvailable, borrow, returnCopy, matchesAuthor, matchesTitle, isNew, reserve|—|
|BookLoan|8: id_, book_ Book*, student_ Student*, borrowedAt_, dueDate_, returnedAt_, finePerDay_, status_|6: isOverdue, overdueDays, calculateFine, returnBook, extend, isActive|Book, Student|
|Dormitory|7: id_, address_, totalRooms_, warden_ Teacher*, pricePerMonth_, phone_, rooms_ DormRoom[]|7: addRoom, findFreeRoom, occupancyRate, freeBeds, assignStudent, evictStudent, monthlyRevenue|Teacher, DormRoom, Student|
|DormRoom|7: number_, dormitory_ Dormitory*, capacity_, floor_, gender_, residents_ Student[], status_|7: addResident, removeResident, hasFreeBed, isFull, freeBeds, containsStudent, monthlyPrice|Dormitory, Student|
|Notification|9: id_, recipient_ Student*, title_, text_, createdAt_, channel_, status_, readAt_, priority_|7: markRead, send, isRead, isUrgent, daysOld, matchesChannel, summary|Student|
|NotificationService|3: notifications_ Notification[], sentCount_, failedCount_|7: notifyStudent, notifyGroup, markAllRead, unreadFor, pendingCount, purgeOlderThan, deliveryRate|Notification, Student, StudentGroup|
|Event|11: id_, title_, description_, startAt_, endAt_, organizer_ Teacher*, location_ Classroom*, maxParticipants_, participants_ Student[], attended_ Student[], status_|9: register, unregister, markAttended, isFull, isUpcoming, durationMinutes, attendancePercent, conflictsWith, cancel|Teacher, Classroom, Student, Event|
|ProgressReport|9: id_, student_ Student*, semester_ Semester*, generatedAt_, averageScore_, debtsCount_, attendancePercent_, ratingPlace_, recommendations_|6: buildFrom, isSuccessful, hasDebts, summaryText, needsImprovement, compareWith|Student, Semester, GradeBook, AttendanceJournal, ProgressReport|
|StudentDirectory|3: students_ Student[], groups_ StudentGroup[], teachers_ Teacher[]|10: addStudent, addGroup, addTeacher, findById, findByName, filterByGroup, filterByCourse, topStudents, studentsWithDebt, exportCsv|Student, StudentGroup, Teacher|

## 11. Exceptions (16 classes)

All headers are in `include/studentis/exceptions/`. `StudentInfoException` publicly inherits `std::runtime_error` and has `explicit StudentInfoException(const std::string&)`. Every other class publicly inherits `StudentInfoException` and uses `using StudentInfoException::StudentInfoException;`.

|Exception|Thrown when|
|---|---|
|StudentInfoException|base class for one `catch` of all domain errors|
|InvalidStudentDataException|empty required name/ID, malformed date, invalid status or contacts|
|StudentNotFoundException|required student lookup fails in University/StudentGroup/StudentDirectory|
|GroupNotFoundException|required group lookup fails or transfer target is missing|
|GroupFullException|StudentGroup.addStudent exceeds maxSize_|
|DisciplineNotFoundException|required discipline lookup fails in Department/Curriculum|
|PrerequisiteNotMetException|Discipline.canStudy finds an unpassed prerequisite|
|ScheduleConflictException|overlapping lessons for one group, teacher, or classroom|
|ClassroomCapacityException|group/event exceeds room capacity or room lacks required equipment|
|InvalidGradeException|score outside 0..100, submission score above maxScore_, or invalid five-scale grade|
|ExamRegistrationException|duplicate/inactive/wrong-group/late exam registration|
|AttendanceException|invalid attendance status, student, or lesson in AttendanceJournal.mark|
|InsufficientFundsException|non-positive money operation, overdraft, or payment above remaining debt|
|PaymentNotFoundException|required payment lookup/registration refers to an unknown payment|
|DocumentExpiredException|expired/blocked card, certificate, or library card is used|
|DormitoryFullException|DormRoom.addResident exceeds capacity or violates room gender|

## 12. Cross-class behavior contracts

Implement these rules exactly; they are covered by tests.

- `Student.status_`: `active`, `academic_leave`, `expelled`, `graduated`. Only `active` students may register for exams/events or receive scholarships.
- Grade scale: scores are 0..100. `isPass` is `>= 60`; `isExcellent` is `>= 85`; `isFail` is `< 60`; `toFiveScale` maps `<60 → 2`, `60..69 → 3`, `70..84 → 4`, `>=85 → 5`.
- `Discipline.canStudy(const Student&)`: every prerequisite must have a passing `Grade` for that student. Return `false` when a prerequisite is missing/failing; use `PrerequisiteNotMetException` only in operations that require enrollment rather than a boolean query.
- `StudentGroup.addStudent`: duplicate pointer is a no-op; if `activeCount() >= maxSize_`, throw `GroupFullException`; otherwise append and set the student's `group_`.
- `Lesson.overlaps`: same date and `start < other.end && other.start < end`. `Schedule.addLesson` rejects overlap for the same group, course teacher, or classroom with `ScheduleConflictException`.
- `Classroom.fits`: `group.activeCount() <= capacity_`; `supports` requires computers for `lab` and projector for `lecture` only when the course material marks it mandatory.
- `TuitionContract.yearlyTotal = annualAmount_ * (1 - discountPercent_ / 100.0)`; `semesterAmount = yearlyTotal / 2`; validate `startDate_ <= endDate_` and `0 <= discountPercent_ <= 100`.
- `FinanceAccount.balance_` is signed money: `charge(amount)` subtracts, confirmed `registerPayment` adds, `debt()` returns `max(0, -balance_)`. Reject non-positive amounts with `InsufficientFundsException`.
- `StudentFine.remaining = max(0, amount_ - paidAmount_)`; `addPayment` rejects overpayment; `isOverdue` means unpaid and `dueDate_ < date`.
- `BookLoan.overdueDays(date) = max(0, daysBetween(dueDate_, date))` for active loans; `calculateFine = overdueDays * finePerDay_`; `returnBook` sets `returnedAt_`, status `returned`, and increments `Book.copiesAvailable_`.
- `DormRoom.addResident`: duplicate is a no-op; full room throws `DormitoryFullException`; a non-`mixed` room must match the student's gender.
- Document validity (`StudentCard`, `Certificate`, `LibraryCard`) requires active status and `issuedAt_ <= date <= expiresAt_`; operations requiring validity throw `DocumentExpiredException` otherwise.
- `Thesis.isAllowedToDefend`: submitted, defense date set, `plagiarismPercent_ <= 20`, and status is not `failed`.
- `Scholarship.isEligible`: active student; academic type also requires `GradeBook.average() >= minAverage_`, no academic debts, and unexcused absences `<= maxAbsences_`; social type ignores average but still enforces the absence limit.
- `AttendanceJournal.mark` accepts only `present`, `absent`, `excused`, `late`; rates are `present + late + excused` divided by all entries, as percentages 0..100.
- `Event.register` returns `false` for duplicates, full events, inactive students, or cancelled events; otherwise appends and returns `true`.
- `ProgressReport.buildFrom` fills average, debt count, attendance percentage, and recommendations (`"нет замечаний"` only when average >= 85, no debts, attendance >= 90).

## 13. Tests and demo

Use Catch2 `TEST_CASE`/`SECTION` in Russian, matching Lab 2. Every module test file must cover normal behavior, boundary values, and at least one thrown exception. Include these concrete cases:

1. **core**: group `maxSize_=1`: first `addStudent` succeeds, second throws `GroupFullException`; `Person("1970-01-01")` age checks; curriculum credits recalculation.
2. **academic**: lessons `2026-09-01 10:00..11:30` and `11:00..12:00` conflict, `12:00..13:00` does not; prerequisite fails without a grade and passes with grade 80; assignment submission above `maxScore_` throws `InvalidGradeException`.
3. **assessment**: score 85 is excellent, 72 maps to 4, 59 maps to 2; duplicate exam registration throws `ExamRegistrationException`; journal with present/late/absent/excused yields 75%.
4. **finance**: contract `annualAmount_=100000`, `discountPercent_=10` gives `yearlyTotal()==90000` and `semesterAmount()==45000`; account charge 50000 then confirmed payment 20000 gives `debt()==30000`; overpayment throws `InsufficientFundsException`.
5. **documents**: expired `StudentCard` is invalid and use throws `DocumentExpiredException`; thesis plagiarism 15 is defendable, 25 is not; transfer `apply()` moves the student to the target group.
6. **services**: loan due `2026-01-10`, checked `2026-01-15`, `finePerDay_=10` gives fine 50; room capacity 2 has 50% occupancy after one resident and third resident throws `DormitoryFullException`; report grades 80/90 and 3 attended of 4 gives average 85 and attendance 75.

`src/main.cpp` is a small interactive Russian menu (like Lab 2), demonstrating: enrollment and group transfer; curriculum and schedule conflict handling; grades/attendance/progress report; tuition contract/payment/scholarship; student card/thesis; library loan fine and dormitory assignment. Construct objects in `main`, keep them alive for the whole demo, and catch all domain errors with `catch (const studentis::StudentInfoException&)`.

## 14. README and acceptance checks

Create `Lab_3/README.md` in Russian, following `Lab_2/README.md`: project purpose, structure, one table per module with `Класс | Поля | Методы | Ассоциации`, an exceptions section, and final statistics. Use actual implemented counts; the inventory above should yield 70 classes, 399 fields, 383 behaviors, 16 exceptions, and more than 30 associations. State the assignment minima explicitly and confirm each is met.

Run from the repository root:

```bash
cmake -S Lab_3/StudentInfo -B Lab_3/StudentInfo/build -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=ON
cmake --build Lab_3/StudentInfo/build
ctest --test-dir Lab_3/StudentInfo/build --output-on-failure
cmake --build Lab_3/StudentInfo/build --target coverage
./Lab_3/StudentInfo/build/studentinfo_app
doxygen Lab_3/StudentInfo/Doxyfile
```

Definition of done:

- all targets build and `studentinfo_tests` passes;
- library line coverage is at least 90%;
- demo runs without uncaught exceptions;
- Doxygen generates `Lab_3/StudentInfo/doc/html/index.html` without errors;
- README contains all 70 classes and all 16 exceptions with truthful counts;
- `git status --short` shows only intended Lab 3 additions; no Lab 1/Lab 2 files are modified.
