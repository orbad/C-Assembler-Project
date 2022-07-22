#include "header.h"

/* Struct decleration */
typedef struct Node{
    String line;
    struct Node * next;
}Node;

typedef struct LinkedList{
    String macroName;
    struct Node * head;
    struct Node * tail;
}LinkedList;

typedef struct Table {
    LinkedList macros;
} Table;

void free_list(LinkedList *list);
int search_list(LinkedList *list, String str);
void print_macro(LinkedList *list, String str);
