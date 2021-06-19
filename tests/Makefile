# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα tets για την υλοποίηση αυτή.

# Υλοποιήσεις μέσω HashTable: ADTMap
#
UsingHashTable_ADTMap_test_OBJS	= ADTMap_test.o $(MODULES)/UsingHashTable/ADTMap.o $(MODULES)/MyStudies/ADTSet.o

# ADTGraph
#
# UsingAdjacencyLists_ADTGraph_test_OBJS = ADTGraph_test.o ...

# MyStudies
#
MyStudies_test_OBJS = MyStudies_test.o $(MODULES)/MyStudies/MyStudies.o $(MODULES)/MyStudies/ADTSet.o $(MODULES)/ADTMap.o $(MODULES)/UsingLinkedList/ADTList.o

# Ο βασικός κορμός του Makefile
include ../common.mk