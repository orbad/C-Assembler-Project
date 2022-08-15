#ifndef __PRE_PROCCESOR_H__
#define __PRE_PROCCESOR_H__

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

void removeTails(char *line);
void free_list(LinkedList *list);
void free_list_of_lists(Table *table);
int search_list(Table *table, String str);
LinkedList* find_macro(Table *table, String str);
void print_macro(LinkedList *list, FILE *file);

#endif