/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hash Table με open addressing (linear probing)
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "ADTMap.h"
#include "ADTSet.h"

// Το μέγεθος του Hash Table ιδανικά θέλουμε να είναι πρώτος αριθμός σύμφωνα με την θεωρία.
// Η παρακάτω λίστα περιέχει πρώτους οι οποίοι έχουν αποδεδιγμένα καλή συμπεριφορά ως μεγέθη.
// Κάθε re-hash θα γίνεται βάσει αυτής της λίστας. Αν χρειάζονται παραπάνω απο 1610612741 στοχεία, τότε σε καθε rehash διπλασιάζουμε το μέγεθος.
int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

// Χρησιμοποιούμε open addressing, οπότε σύμφωνα με την θεωρία, πρέπει πάντα να διατηρούμε
// τον load factor του  hash table μικρότερο ή ίσο του 0.5, για να έχουμε αποδoτικές πράξεις
#define MAX_LOAD_FACTOR 0.5

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;		// Το κλειδί που χρησιμοποιείται για να hash-αρουμε
	Pointer value;  	// Η τιμή που αντισοιχίζεται στο παραπάνω κλειδί
	Map owner;			// To map
};


// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	Set* array;					// Ο πίνακας που θα χρησιμοποιήσουμε για το map (remember, φτιάχνουμε ένα hash table)
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει.
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	CompareFunc compare;		// Συνάρτηση για σύγκρηση δεικτών, που πρέπει να δίνεται απο τον χρήστη
	HashFunc hash_function;		// Συνάρτηση για να παίρνουμε το hash code του κάθε αντικειμένου.
	DestroyFunc destroy_key;	// Συναρτήσεις που καλούνται όταν διαγράφουμε έναν κόμβο απο το map.
	DestroyFunc destroy_value;
};

static int compare_map_nodes(MapNode a, MapNode b) {

	return a->owner->compare(a->key, b->key);
}

// Συνάρτηση που καταστρέφει ένα map node
static void destroy_map_node(MapNode node) {
	if (node->owner->destroy_key != NULL)
		node->owner->destroy_key(node->key);

	if (node->owner->destroy_value != NULL)
		node->owner->destroy_value(node->value);

	free(node);
}

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	// Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το hash table
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(*map->array));

	map->size = 0;
	map->compare = compare;

	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	for (uint i = 0 ; i < map->capacity ; i++){
		map->array[i] = set_create((CompareFunc)compare_map_nodes, (DestroyFunc)destroy_map_node);
	}
	
	return map;
}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}

// Συνάρτηση για την επέκταση του Hash Table σε περίπτωση που ο load factor μεγαλώσει πολύ.
static void rehash(Map map) {
	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	Set* old_array = map->array;

	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int prime_no = sizeof(prime_sizes) / sizeof(int);	// το μέγεθος του πίνακα
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}
	// Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	// Δημιουργούμε ένα μεγαλύτερο hash table
	map->array = malloc(map->capacity * sizeof(*map->array));
	// Τοποθετούμε ΜΟΝΟ τα entries που όντως περιέχουν ένα στοιχείο (το rehash είναι και μία ευκαιρία να ξεφορτωθούμε τα deleted nodes)
	map->size = 0;

	for (int i = 0 ; i < map->capacity ; i++){
		map->array[i] = set_create((CompareFunc)compare_map_nodes, (DestroyFunc)destroy_map_node);
	}

	for (int i = 0; i < old_capacity; i++){
		for (SetNode j = set_first(old_array[i]) ; j != SET_EOF ; j = set_next(old_array[i], j)){
			MapNode node = set_node_value(old_array[i], j);
			map_insert(map, node->key, node->value);
		}
	}

	for (int i = 0 ; i < old_capacity ; i++){
		DestroyFunc oldkey = map_set_destroy_key(map, NULL);
		DestroyFunc oldvalue = map_set_destroy_value(map, NULL);
		set_destroy(old_array[i]);
		map_set_destroy_key(map, oldkey);
		map_set_destroy_value(map, oldvalue);
	}

	//Αποδεσμεύουμε τον παλιό πίνακα ώστε να μήν έχουμε leaks
	
	free(old_array);
}

// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.

void map_insert(Map map, Pointer key, Pointer value) {
	// Σκανάρουμε το Hash Table μέχρι να βρούμε διαθέσιμη θέση για να τοποθετήσουμε το ζευγάρι,
	// ή μέχρι να βρούμε το κλειδί ώστε να το αντικαταστήσουμε.
	bool already_in_map = false;
	MapNode node = NULL;
	uint pos = map->hash_function(key) % map->capacity;

	MapNode tempor = malloc(sizeof(*tempor));
	tempor->key = key;
	tempor->value = value;
	tempor->owner = map;

	node = set_find(map->array[pos], tempor);
	
	free(tempor);

	if (node == NULL){
		MapNode temp = malloc(sizeof(*temp));
		temp->key = key;
		temp->value = value;
		temp->owner = map;
		set_insert(map->array[pos], temp);
	}
	else{
		already_in_map = true;
	}
	
	// Σε αυτό το σημείο, το node είναι ο κόμβος στον οποίο θα γίνει εισαγωγή.
	if (already_in_map) {
		// Αν αντικαθιστούμε παλιά key/value, τa κάνουμε destropy
		if (node->key != key && map->destroy_key != NULL)
			map->destroy_key(node->key);
			

		if (node->value != value && map->destroy_value != NULL)
			map->destroy_value(node->value);

		node->key = key;
		node->value = value;	

	} else {
		// Νέο στοιχείο, αυξάνουμε τα συνολικά στοιχεία του map
		map->size++;
	}

	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash
	float load_factor = (float)map->size / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	
	MapNode node = map_find_node(map, key);
	if (node == MAP_EOF)
		return false;

	uint pos = map->hash_function(key) % map->capacity;
	
	map->size--;
	set_remove(map->array[pos], node);

	return true;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node != MAP_EOF)
		return node->value;
	else
		return NULL;
	return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	for (int i = map->capacity - 1; i >= 0 ; i--)
		set_destroy(map->array[i]);

	free(map->array);
	free(map);
	
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	for (int i = 0; i < map->capacity; i++){
		SetNode node = set_first(map->array[i]);
		if (node != NULL){
			return set_node_value(map->array[i], node);
		}
	}

	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	uint pos = map->hash_function(node->key) % map->capacity;

	SetNode snode = set_find_node(map->array[pos], node);
	
	snode = set_next(map->array[pos], snode);
	
	if (snode == SET_EOF){
		pos++;
		while (pos < map->capacity){
			if (set_size(map->array[pos]) != 0){
				MapNode mnode = set_node_value(map->array[pos], set_first(map->array[pos]));
				return mnode;
			}
			pos++;
		}
		return MAP_EOF;
	}

	MapNode to_return = set_node_value(map->array[pos], snode);
	return to_return;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	// Διασχίζουμε τον πίνακα, ξεκινώντας από τη θέση που κάνει hash το key, και για όσο δε βρίσκουμε EMPTY
	uint pos = map->hash_function(key) % map->capacity;

	MapNode node = malloc(sizeof(*node));
	node->key = key;
	node->owner = map;

	MapNode found = set_find(map->array[pos], node);

	free(node);

	if (found != NULL){
		return found;
	}

	return MAP_EOF;
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}