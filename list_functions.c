#include "pre_proccesor.h"
#include "header.h"

/*removing the whitespaces at the end of each sentence*/
void removeTails(char *line)
{
    int index, i;
    /* Set default index */
    index = NEUTRAL;

    /* Find last index of non-white space character */
    i = 0;
    while (line[i] != '\0')
    {
        if ((line[i] != ' ') && (line[i] != '\t') && (line[i] != '\n'))
            index = i;
        i++;
    }

    line[index + 1] = '\0';
    return;
}

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
    return;
}

void free_list_of_lists(Table *table)
{
    LinkedList *curr = table->head;
    LinkedList *tmp;

    while (curr != NULL)
    {
        tmp = curr;
        curr = curr->next;
        free_list(tmp);
    }
    return;
}

/* Checks if the macro is already registered in the Macro Table.
   returns 0 if the macro is a new one
   returns -1 if the macro is already registered */
int search_list(Table *table, String str)
{
    LinkedList *curr;
    int res = -1;
    if (table != NULL)
    {
        curr = table->head;
        while (curr != NULL)
        {
            res = memcmp((curr->macroName), str, strlen(str));;
            if (res == 0)
            {
                printf("Macro is already defined");
                return -1; /* Macro is already registerd */
            }
            curr = table->head->next;
        }
    }
    else
    {
        return 0; /* Macro insert is valid */
    }
}

LinkedList *find_macro(Table *table, String str)
{
    LinkedList *curr = table->head;
    int res = 0;
    while (curr != NULL)
    {
        res = memcmp((curr->macroName), str, strlen(str) -1);
        if (res == 0)
        {
            break;
        }
        curr = curr->next;
    }
    return curr;
}

void print_macro(LinkedList *list, FILE *file)
{
    Node *curr = list->head;

    while (curr != NULL)
    {
        fputs(curr->line, file);
        curr = curr->next;
    }
    return;
}