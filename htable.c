#include <stdio.h>
#include <stdlib.h>
#include "htable.h"

/**
 * capacity: The size of the hashtable
 * words: keys
 * lists: values
 * <k,v> = <word, int array>
 */
struct htablerec {
    int capacity;
    char **words;
    int **lists;
};

/**
 * initialises a new hashtable
 */
htable htable_new(int capacity) {
    htable h = malloc(sizeof *h);
    h->capacity = capacity;
    h->words = malloc(sizeof h->words[0] * capacity);
    h->lists = malloc(sizeof h->lists[0] * capacity);
    for (int i = 0; i < capacity; i++) {
        h->words[i] = NULL;
        h->lists[i] = NULL;
    }
    return h;
}

/**
 * FREEDOM
 */
void htable_free(htable h) {
    for (int i = 0; i < h->capacity; i++) {
        free(h->words[i]);
        free(h->lists[i]);
    }
    free(h->words);
    free(h->lists);
    free(h);
}

/**
 * hashing function using djb2 described at
 * http://www.cse.yorku.ca/~oz/hash.html
 */
static unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

static unsigned int htable_step(htable h, unsigned long key) {
    return 1 + (key % (h->capacity - 1));
}

/**
 * inserts into the hashtable with k = str, v = list
 */
int htable_put(htable h, char *str, int *list) {
    int key = hash(str)%h->capacity;
    int step = htable_step(h, hash(str));

    if (h->words[key] == NULL) {
        h->words[key] = emalloc((strlen(str)+1) * sizeof str[0]);
        strcpy(h->words[key], str);
        h->lists[key] = list; // does this work?
        return 1;
    } else { // collision resolution using double hashing
        int key_i;
        for (int i = 0; i < h->capacity; i++) {
            key_i = (key + i*step)%h->capacity;
            if (h->words[key_i] == NULL) {
                h->words[key_i] = emalloc((strlen(str)+1) * sizeof str[0]);
                strcpy(h->words[key_i], str);
                h->lists[key_i] = list; // does this work?
                return 1;
            }
        }
    }
    return 0;
}

/**
 * returns the associated docs
 */
int* htable_get(htable h, char *str) {
    int key = hash(str)%h->capacity;
    int step = htable_step(h, hash(str));
    int index = key;
    
    for (int i = 0; i < h->capacity && h->words[index] != NULL; i++) {
        if (strcmp(str, h->words[index]) == 0) {
            return h->lists[index];
        } else {
            index = (key + i*step)%h->capacity;
        }
    }
}
