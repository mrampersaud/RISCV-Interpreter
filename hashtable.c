#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"

struct hashtable
{
    linkedlist_t ** buckets;
    int length;
};

/**
 * Hash function to hash a key into the range [0, max_range)
 */
static int hash(int key, int max_range)
{
    key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets)
{
    hashtable_t *table = malloc(sizeof(hashtable_t));
    table->buckets = malloc(num_buckets*sizeof(linkedlist_t*)); 
    for (int i = 0; i < num_buckets; i++){ // iterates through bucket space and initializes each list
        table->buckets[i] = ll_init();
    }
    table->length = num_buckets;
    return table;
}

void ht_free(hashtable_t *table)
{
    int length = table->length;
    linkedlist_t * curr;
    for (int i = 0; i < length; i++){
        curr = table->buckets[i];
        ll_free(curr);
    } 
    free(table->buckets);
    free(table);
}

void ht_add(hashtable_t *table, int key, int value)
{
    int index = hash(key, table->length);
    linkedlist_t * bucket = table->buckets[index];
    ll_add(bucket, key, value);
}

int ht_get(hashtable_t *table, int key)
{
    int index = hash(key, table->length);
    linkedlist_t * bucket = table->buckets[index];
    return ll_get(bucket, key);
}

int ht_size(hashtable_t *table)
{
    int capacity = 0;
    linkedlist_t * curr;
    int length = table->length;
    for (int i = 0; i < length; i++){
        curr = table->buckets[i];
        capacity += ll_size(curr);
    }
    return capacity;
}
