#include <stdlib.h>

#include "list.h"

struct list *list_create()
{
    struct list *l = (struct list *) malloc (sizeof (struct list));
    l->head = NULL;
    l->tail = NULL;
    return l;
}

void list_insert (struct list *l, void *data)
{
    if (l->head == NULL)
    {
        l->head = (struct node *) malloc (sizeof (struct node));
        l->head->data = data;
        l->head->prev = NULL;
        l->head->next = NULL;
        l->tail = l->head;
    }
    else
    {
        struct node *new =  malloc (sizeof (struct node));

        new->data = data;
        new->next = NULL;
        new->prev = l->tail;
        l->tail->next = new;
        l->tail = new;
    }
}

void list_delete_node (struct list *l, struct node *N)
{
    if (!l || !l->head) return;

    struct node *cur = l->head;

    if (cur == N)
    {
        l->head = cur->next;
        free(cur);
        return;
    }
    cur = cur->next;

    while (cur != N && cur != NULL)
    {
        cur = cur->next;
    }

    // not found
    if (cur == NULL)
        return;

    // found as last
    if (cur == l->tail)
    {
        l->tail = cur->prev;
        cur->prev->next = NULL;
        free(N);
        return;
    }

    // middle
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    free(N);
}

void list_destroy (struct list *l)
{
    if (!l) return;
    struct node *cur = l->head;
    while (cur != NULL)
    {
        struct node *next = cur->next;
        free (cur);
        cur = next;
    }
    free (l);
}
