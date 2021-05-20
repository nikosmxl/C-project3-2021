//////////////////////////////////////////////////////////////////
//
// Unit tests for MyStudies.h
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include <limits.h>
#include "math.h"

#include "MyStudies.h"

// test students
struct student students[] = {
	// city = house
	{ .id = 1,  .name = "Daenerys", .city = "Targaryen", .year = 301, .gpa = 6.0 },
	{ .id = 2,  .name = "Jaime",    .city = "Lannister", .year = 302, .gpa = 5.1 },
	{ .id = 3,  .name = "Cersei",   .city = "Lannister", .year = 302, .gpa = 7.1 },
	{ .id = 4,  .name = "Ned",      .city = "Stark",     .year = 298, .gpa = 5.5 },
	{ .id = 5,  .name = "Rhaegar",  .city = "Targaryen", .year = 281, .gpa = 5.6 },
	{ .id = 6,  .name = "Sansa",    .city = "Stark",     .year = 301, .gpa = 7.3 },
	{ .id = 8,  .name = "Tyrion",   .city = "Lannister", .year = 299, .gpa = 9.5 },
	{ .id = 9,  .name = "Tywin",    .city = "Lannister", .year = 301, .gpa = 7.2 },
	{ .id = 10, .name = "Arya",     .city = "Stark",     .year = 301, .gpa = 8.4 },
	{ .id = 11, .name = "Bran",     .city = "Stark",     .year = 301, .gpa = 8.1 },
	{ .id = 12, .name = "Sandor",   .city = "Clegane",   .year = 299, .gpa = 5.5 },
	{ .id = 13, .name = "Aegon",    .city = "Targaryen", .year = 296, .gpa = 6.2 },
	{ .id = 14, .name = "Joffrey",  .city = "Baratheon", .year = 301, .gpa = 4.9 },
	{ .id = 15, .name = "Petyr",    .city = "Baelish",   .year = 300, .gpa = 9.1 },
	{ .id = 16, .name = "Euron",    .city = "Greyjoy",   .year = 300, .gpa = 6.5 },
	{ .id = 17, .name = "Theon",    .city = "Greyjoy",   .year = 300, .gpa = 5.2 },
	{ .id = 18, .name = "Oberyn",   .city = "Martell",   .year = 301, .gpa = 6.1 },
	{ .id = 19, .name = "Robert",   .city = "Baratheon", .year = 298, .gpa = 5.2 },
	{ .id = 20, .name = "Jorah",    .city = "Mormont",   .year = 299, .gpa = 8.5 },

};
int student_no = sizeof(students)/sizeof(struct student);


// Ελέγχει ότι οι εγγραφές στη λίστα result περιέχουν τα ids στον πινακα ids
// με τη συγκεκριμένη σειρά.
void check_student_list(List result, int ids[], int size) {
	TEST_ASSERT(list_size(result) == size);

	int i = 0;
	for (ListNode node = list_first(result); node != LIST_EOF; node = list_next(result, node)) {
		Student stud = list_node_value(result, node);
		TEST_ASSERT(ids[i] == stud->id);
		i++;
	}

	list_destroy(result);
}


void test_init(void) {
	ms_init();

	TEST_ASSERT(ms_count_all() == 0);

	ms_destroy();
}

void test_insert(void) {
	ms_init();

	for(int i = 0; i < student_no; i++) {
		// Εισαγωγή της εγγραφής i
		TEST_ASSERT( ms_insert_student(&students[i]));	
		TEST_ASSERT(!ms_insert_student(&students[i]));		// exists, returns false

		TEST_ASSERT(ms_count_all() == i+1);
		TEST_ASSERT(ms_get_student(students[i].id) == &students[i]);
	}

	ms_destroy();
}

void test_remove(void) {
	ms_init();

	// insert students
	for(int i = 0; i < student_no; i++)
		ms_insert_student(&students[i]);	

	for(int i = 0; i < student_no; i++) {
		// Delete της εγγραφής i
		TEST_ASSERT(ms_remove_student(students[i].id));	
		TEST_ASSERT(!ms_remove_student(students[i].id));		// already deleted, returns false

		TEST_ASSERT(ms_count_all() == student_no-i-1);
		TEST_ASSERT(ms_get_student(students[i].id) == NULL);
	}

	ms_destroy();
}

void test_count_by_year(void) {
	ms_init();

	// insert students
	for(int i = 0; i < student_no; i++)
		ms_insert_student(&students[i]);	

	TEST_ASSERT(ms_count_by_year(281) == 1);
	TEST_ASSERT(ms_count_by_year(296) == 1);
	TEST_ASSERT(ms_count_by_year(298) == 2);
	TEST_ASSERT(ms_count_by_year(299) == 3);
	TEST_ASSERT(ms_count_by_year(300) == 3);
	TEST_ASSERT(ms_count_by_year(301) == 7);
	TEST_ASSERT(ms_count_by_year(302) == 2);

	ms_destroy();
}

void test_top_by_year(void) {
	ms_init();

	// insert students
	for(int i = 0; i < student_no; i++)
		ms_insert_student(&students[i]);	

	// Για κάθε έτος, ζητάμε τον 1ο καλύτερο, τους 2 πρώτους, τους 3, κλπ
	int ids1[] = {15, 16, 17};
	for(int i = 1; i <= sizeof(ids1)/sizeof(int); i++)
		check_student_list(ms_top_by_year(300, i), ids1, i);

	int ids2[] = {10, 11, 6, 9, 18, 1, 14};
	for(int i = 1; i <= sizeof(ids2)/sizeof(int); i++)
		check_student_list(ms_top_by_year(301, i), ids2, i);

	int ids3[] = {3, 2};
	for(int i = 1; i <= sizeof(ids3)/sizeof(int); i++)
		check_student_list(ms_top_by_year(302, i), ids3, i);

	ms_destroy();
}

void test_bottom_by_year(void) {
	ms_init();

	// insert students
	for(int i = 0; i < student_no; i++)
		ms_insert_student(&students[i]);	

	// Για κάθε έτος, ζητάμε τον 1ο χειρότερο, τους 2 πρώτους, τους 3, κλπ
	int ids1[] = {17, 16, 15};
	for(int i = 1; i <= sizeof(ids1)/sizeof(int); i++)
		check_student_list(ms_bottom_by_year(300, i), ids1, i);

	int ids2[] = {14, 1, 18, 9, 6, 11, 10};
	for(int i = 1; i <= sizeof(ids2)/sizeof(int); i++)
		check_student_list(ms_bottom_by_year(301, i), ids2, i);

	int ids3[] = {2, 3};
	for(int i = 1; i <= sizeof(ids3)/sizeof(int); i++)
		check_student_list(ms_bottom_by_year(302, i), ids3, i);

	ms_destroy();
}

// Οι πράξεις με float δεν έχουν καλή ακρίβεια, η συνήθης πρακτική
// είναι να ελέγχουμε ότι είναι απλά κοντά στην σωστή τιμή.
//
static bool float_equal(float a, float b) {
	return fabs(a - b) < 0.00001;
}

void test_average_by_year(void) {
	ms_init();

	// insert students
	for(int i = 0; i < student_no; i++)
		ms_insert_student(&students[i]);

	TEST_ASSERT(float_equal(ms_average_by_year(281), 5.6));
	TEST_ASSERT(float_equal(ms_average_by_year(296), 6.2));
	TEST_ASSERT(float_equal(ms_average_by_year(298), 5.35));
	TEST_ASSERT(float_equal(ms_average_by_year(299), 7.83333333));
	TEST_ASSERT(float_equal(ms_average_by_year(300), 6.93333333));
	TEST_ASSERT(float_equal(ms_average_by_year(301), 6.857143));
	TEST_ASSERT(float_equal(ms_average_by_year(302), 6.1));

	ms_destroy();
}

void test_city_rank(void) {
	ms_init();

	// insert students
	for(int i = 0; i < student_no; i++)
		ms_insert_student(&students[i]);	

	// Στην περίπτωση ισότητας, η υλοποίηση μπορεί να επιστρέφει τις πόλεις με οποιαδήποτε σειρά.
	TEST_ASSERT(
		(strcmp(ms_city_rank(1), "Stark") == 0 && strcmp(ms_city_rank(2), "Lannister") == 0) ||
		(strcmp(ms_city_rank(2), "Stark") == 0 && strcmp(ms_city_rank(1), "Lannister") == 0)
	);
	TEST_ASSERT(strcmp(ms_city_rank(3), "Targaryen") == 0);
	TEST_ASSERT(
		(strcmp(ms_city_rank(4), "Baratheon") == 0 && strcmp(ms_city_rank(5), "Greyjoy") == 0) ||
		(strcmp(ms_city_rank(5), "Baratheon") == 0 && strcmp(ms_city_rank(4), "Greyjoy") == 0)
	);

	ms_destroy();
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "ms_init", test_init },
	{ "ms_insert_student", test_insert },
	{ "ms_remove_student", test_remove },
	{ "ms_count_by_year", test_count_by_year },
	{ "ms_top_by_year", test_top_by_year },
	{ "ms_bottom_by_year", test_bottom_by_year },
	{ "ms_average_by_year", test_average_by_year },
	{ "ms_city_rank", test_city_rank },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};