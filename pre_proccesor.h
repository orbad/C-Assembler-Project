#include "header.h"

/* Struct decleration */

/* Each node represets a line in the LinkedList */
typedef struct Node{
    String line;
    struct Node * next;
}Node;

typedef struct LinkedList{
    String macroName;
    struct Node * head;
    struct Node * tail;
    struct LinkedList * next;
}LinkedList;

typedef struct Table{
    struct LinkedList * head;
    struct LinkedList * tail; 
}Table;

void free_list(LinkedList *list);
int search_list(LinkedList *list, String str);
void print_macro(LinkedList *list, String str);
void free_list_of_lists(Table *table);