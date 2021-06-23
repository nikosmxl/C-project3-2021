///////////////////////////////////////////////////////////////////
//
// MyStudies
//
// Module που παρέχει στατιστικά φοιτητών.
//
///////////////////////////////////////////////////////////////////

#include "MyStudies.h"
#include <stdlib.h>
#include <string.h>
#include "ADTSet.h"
#include "ADTMap.h"

bool data = 0;

int* create_int(int value) {
    int* pointer = malloc(sizeof(int));         // δέσμευση μνήμης
    *pointer = value;                           // αντιγραφή του value στον νέο ακέραιο
    return pointer;
}

struct years_node
{
    int year;           
    int counter;
    float grades_sum;
    float gpa_of_students;
};
typedef struct years_node* YearsNode;

struct cities_node
{
    int counter;
    String city;
};
typedef struct cities_node* CitiesNode;

struct cities_node_2
{
    String city;
    int counter;
};
typedef struct cities_node* CitiesNode2;

CitiesNode2 create_citynode2(String city) {
    CitiesNode2 pointer = malloc(sizeof(*pointer));
    pointer->city = city;
    return pointer;
}

CitiesNode create_citynode(CitiesNode2 cn2) {
    CitiesNode pointer = malloc(sizeof(*pointer));
    pointer->counter = cn2->counter;
    pointer->city = cn2->city;
    return pointer;
}

YearsNode create_yearnode(int year) {
    YearsNode pointer = malloc(sizeof(*pointer));
    pointer->year = year;
    return pointer;
}

static int compare_students(Pointer a, Pointer b){
    Student as = a;
    Student bs = b;

    return as->id - bs->id;
}

static int compare_gpas(Pointer a, Pointer b){
    Student as = a;
    Student bs = b;

    if (as->gpa > bs->gpa){
        return 1;
    }
    else if (as->gpa == bs->gpa){
        return compare_students(a, b);
    }
    else{
        return -1;
    }
}

static int compare_years(Pointer a, Pointer b){
    YearsNode as = a;
    YearsNode bs = b;

    return as->year - bs->year;
}

static int compare_cities(Pointer a, Pointer b){
    CitiesNode as = a;
    CitiesNode bs = b;

    if (as->counter == bs->counter){
        return strcmp(as->city, bs->city);
    }

    return as->counter - bs->counter;
}

static int compare_cities2(Pointer a, Pointer b){
    CitiesNode2 as = a;
    CitiesNode2 bs = b;

    return strcmp(as->city, bs->city);
}

static int compare_ints(Pointer a, Pointer b){
    return *(int*)a - *(int*)b;
}

struct mystudies
{ 
    Set students;       // Περιεχει στοιχεια τυπου Student
    int count;          // Περιεχει τον συνολικό αριθμό φοιτητών 
    Set years;          // Περιεχει στοιχεια τυπου YearsNode
    Set cities;         // Περιεχει στοιχεια τυπου CitiesNode
    Set cities2;        // Περιεχει στοιχεια τυπου CitiesNode2
    Map gpa_students;   // Περιεχει Set των μαθητων καθε year, ταξινομημενο συμφωνα με τους μεσους ορους
};
typedef struct mystudies* Studies;

Studies studies;

void ms_init(){
    if (data == 1){
        ms_destroy();
    }
    
    data = 1;
    studies = malloc(sizeof(*studies));

    studies->students = set_create(compare_students, NULL);
    studies->count = 0;
    studies->years = set_create(compare_years, free);
    studies->cities = set_create(compare_cities, free);
    studies->cities2 = set_create(compare_cities2, free);
    studies->gpa_students = map_create(compare_ints, free, NULL);
}

void ms_destroy(){
    if (data == 0){
        return;
    }

    data = 0;

    for (MapNode mn =  map_first(studies->gpa_students) ; mn != MAP_EOF ; mn = map_next(studies->gpa_students, mn)){
        Set set = map_node_value(studies->gpa_students, mn);
        set_destroy(set);
    }

    map_destroy(studies->gpa_students);
    set_destroy(studies->cities);
    set_destroy(studies->cities2);
    set_destroy(studies->years);
    set_destroy(studies->students);
    free(studies);
}

bool ms_insert_student(Student student){
    int old_size = set_size(studies->students);
    set_insert(studies->students, student);
    bool to_return = set_size(studies->students) - old_size;
    studies->count += to_return;
    
    if (to_return == 1){
        YearsNode temp = create_yearnode(student->year);

        YearsNode yn = set_find(studies->years, temp);

        free(temp);
        if (yn == NULL){
            YearsNode yn_new = create_yearnode(student->year);
            set_insert(studies->years, yn_new);
            
            yn_new->grades_sum = student->gpa;
            yn_new->counter = 1;
            yn_new->gpa_of_students = student->gpa;
        }
        else{
            yn->grades_sum += student->gpa;
            yn->counter++;
            yn->gpa_of_students = yn->grades_sum/yn->counter;
        }

        CitiesNode2 temp2 = create_citynode2(student->city);

        CitiesNode2 cn2 = set_find(studies->cities2, temp2);

        free(temp2);

        if (cn2 == NULL){
            CitiesNode2 cn_new = create_citynode2(student->city);
            set_insert(studies->cities2, cn_new);
            
            cn_new->counter = 1;

            set_insert(studies->cities, create_citynode(cn_new));
        }
        else{
            CitiesNode cn = create_citynode(cn2);

            CitiesNode cn_to_remove = set_find(studies->cities, cn);

            free(cn);

            set_remove(studies->cities, cn_to_remove);
            cn2->counter++;

            set_insert(studies->cities, create_citynode(cn2));
        }
        
        MapNode mn = map_find_node(studies->gpa_students, &student->year);
    
        if (mn == NULL){
            map_insert(studies->gpa_students, create_int(student->year), set_create(compare_gpas, NULL));
            Set mn_new = map_find(studies->gpa_students, &student->year);
            set_insert(mn_new, student);
        }
        else{
            Set mn_value = map_node_value(studies->gpa_students, mn);
            set_insert(mn_value, student);
        }
    }

    return to_return;
}

bool ms_remove_student(int id){
    Student stud = ms_get_student(id);

    if (stud != NULL){

        // Φτιαχνω το Count
        studies->count--;

        // Φτιαχνω το Map
        Set set = map_find(studies->gpa_students, &stud->year);

        set_remove(set, stud);
        if (set_size(set) == 0){
            set_destroy(set);
            map_remove(studies->gpa_students, &stud->year);
        }

        // Φτιαχνω τα Cities
        CitiesNode2 cn2 = create_citynode2(stud->city);

        CitiesNode2 cn2_to_change = set_find(studies->cities2, cn2);

        free(cn2);

        CitiesNode cn = create_citynode(cn2_to_change);
        CitiesNode cn_to_del = set_find(studies->cities, cn);
        free(cn);

        set_remove(studies->cities, cn_to_del);

        if (--cn2_to_change->counter == 0){
            set_remove(studies->cities2, cn2_to_change);
        }
        else{
            set_insert(studies->cities, create_citynode(cn2_to_change));
        }

        // Φτιαχνω τα Years
        YearsNode yn = create_yearnode(stud->year);

        YearsNode found = set_find(studies->years, yn);

        free(yn);

        if (--found->counter == 0){
            set_remove(studies->years, found);
        }
        else{
            found->grades_sum -= stud->gpa;
            found->gpa_of_students = found->grades_sum / found->counter;
        }

        // Φτιαχνω και το Set των students
        set_remove(studies->students, stud);
        
        return true;
    }
    
    return false;
}

Student ms_get_student(int id){
    Student stud = malloc(sizeof(*stud));
    stud->id = id;
    Student to_return = set_find(studies->students, stud);
    free(stud);
    return to_return;
}

int ms_count_all(){
    return studies->count;
}

int ms_count_by_year(int year){
    YearsNode node = malloc(sizeof(*node));
    node->year = year;
    YearsNode found = set_find(studies->years, node);

    free(node);

    if (found == NULL){
        return 0;
    }

    return found->counter;
}

List ms_top_by_year(int year, int number){
    List to_return = list_create(NULL);

    Set set = map_find(studies->gpa_students, &year);

    for (int i = 1 ; i <= number ; i++){
        list_insert_next(to_return, list_last(to_return), set_get_at(set, set_size(set) - i));
    }

    return to_return;
}

List ms_bottom_by_year(int year, int number){
    List to_return = list_create(NULL);

    Set set = map_find(studies->gpa_students, &year);

    for (int i = 0 ; i < number ; i++){
        list_insert_next(to_return, list_last(to_return), set_get_at(set, i));
    }

    return to_return;
}

float ms_average_by_year(int year){
    YearsNode found = set_find(studies->years, &year);

    if (found != NULL){
        return found->gpa_of_students;
    }
    return 0;
}

String ms_city_rank(int k){
    if (k > set_size(studies->cities)){
        return NULL;
    }

    CitiesNode cn = set_get_at(studies->cities, set_size(studies->cities) - k);

    return cn->city;
}