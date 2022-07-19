#include "header.h"

/* Struct decleration */
typedef struct node{
    string line;
    struct node * next;
}node;

typedef struct LinkedList{
    struct Node * head;
    struct Node * tail;
}LinkedList;

typedef struct table {
    string name;
    node data;
} table;

void FreeList(LinkedList *list);


