#include "pre_proccesor.h"
#include "header.h"

void free_list(LinkedList *list)
{
    Node *curr = list->head;
    Node *tmp;

    while (curr != NULL)
    {
        tmp = curr;
        curr = curr->next;
        free(tmp);
    }
}

void free_list_of_lists(Table *table){
    LinkedList *curr = table->head;
    LinkedList *tmp;

    while (curr != NULL)
    {
        tmp = curr;
        curr = curr->next;
        free_list(tmp);
    }
}

/* Checks if the macro is already registered in the Macro Table.
   returns 0 if the macro is a new one
   returns -1 if the macro is already registered */
int search_list(Table *table, String str)
{
    LinkedList *curr;
    int res = 0;
    if (table != NULL)
    {
        curr = table->head;
        while (curr != NULL)
        {
            res = strcmp((curr), str);
            curr = table->head->next;
        }
    }
    if (res == 0)
    {
        printf("Macro is already defined");
        return -1; /* Macro is already registerd */
    }
    else
    {
        return 0; /* Macro insert is valid */
    }
}

LinkedList* find_macro(Table *table, String str)
{
    LinkedList *curr = table->head;

    while (curr != NULL)
    {
        if (strcmp((curr->macroName), str) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void print_macro(LinkedList *list, FILE *file){
    Node *curr = list->head;

    while (curr != NULL)
    {
        fputs(curr->line,file);
    }
}