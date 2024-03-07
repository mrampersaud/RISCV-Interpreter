#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

struct linkedlist
{
    struct linkedlist_node *first;
    unsigned int size;

};

struct linkedlist_node
{
    unsigned int key;
    int value;
    struct linkedlist_node* next;
};
typedef struct linkedlist_node linkedlist_node_t;

linkedlist_t *ll_init()
{
    linkedlist_t *list = malloc(sizeof(linkedlist_t));
    list->first = NULL;
    list->size = 0;
    return list;
}

void ll_free(linkedlist_t *list)
{
    int size = list->size;
    linkedlist_node_t * curr = list->first;
    for (int i = 0; i < size; i++){
        free(curr);
        curr = curr->next;
    }
    free(list);
}

void ll_add(linkedlist_t *list, int key, int value)
{
    linkedlist_node_t * node = malloc(sizeof(linkedlist_node_t));
    node->key = key;
    node->value = value;
    int size = list->size;
    if (size == 0) {
        list->first = node;
        node->next = NULL;
        list->size++; //inc only if adding node to list
    } else {
        int replace = 0;
        linkedlist_node_t * curr = list->first; 
        for (int i = 0; i < size; i++){
            if (curr->key == key){ // if current "first" key of list is the same, replace
                replace = 1;
                curr->value = value;
                free(node);
                break;
            } 
            curr = curr->next; 
        }
        if (replace == 0) { //if we didn't replace the key, it does not already exist in ll. add
            node->next = list->first;
            list->first = node;
            list->size++; //inc only if adding
        }
    }

}

int ll_get(linkedlist_t *list, int key)
{
    int size = list->size;
    linkedlist_node_t * curr = list->first;
    for (int i = 0; i < size; i++){
        if (curr->key == key){
            return curr->value;
        }
        curr = curr->next; 
    }
    return 0;
}

int ll_size(linkedlist_t *list)
{
    return list->size;
}
