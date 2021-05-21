///////////////////////////////////////////////////////////////////
//
// MyStudies
//
// Module που παρέχει στατιστικά φοιτητών.
//
///////////////////////////////////////////////////////////////////

#include "MyStudies.h"
#include <stdlib.h>
#include "ADTSet.h"
#include "ADTMap.h"

static int compare_ints(Pointer a, Pointer b){
    return *(int*)a - *(int*)b;
}

struct years_node
{
    int year;
    int counter;
    float gpa_of_students;
};
typedef struct years_node* YearsNode;

struct cities_node
{
    int counter;
    String city;
};
typedef struct cities_node* CitiesNode;

struct gpa_students_node
{
    int year;
    Set gpas;
};
typedef struct gpa_students_node* GpaNode;

struct mystudies
{ 
    Set students;       // Περιεχει στοιχεια τυπου Student
    int count;          // Περιεχει τον συνολικό αριθμό φοιτητών 
    Set years;          // Περιεχει στοιχεια τυπου YearsNode
    Set cities;         // Περιεχει στοιχεια τυπου CitiesNode
    Map gpa_students;   // Περιεχει Set των μαθητων καθε year, ταξινομημενο συμφωνα με τους μεσους ορους
};
typedef struct mystudies* Studies;

void ms_init(){
    Studies studies;
    if (studies != NULL){
        ms_destroy();
    }
    studies = malloc(sizeof(*studies));

    studies->students = set_create(compare_ints, NULL);
    studies->count = 0;
    studies->years = set_create(compare_ints, free); /* free???????*/
    studies->cities = set_create(compare_ints, free); /* free???????*/
    studies->gpa_students = map_create(compare_ints, free, NULL); /* free???????*/
}