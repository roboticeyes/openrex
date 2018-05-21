#pragma once

struct node
{
    void *data;
    struct node *prev;
    struct node *next;
};

struct list
{
    struct node *head;
    struct node *tail;
};

struct list *list_create();
void list_destroy (struct list *l);

void list_insert (struct list *l, void *data);
void list_delete_node (struct list *l, struct node *N);
