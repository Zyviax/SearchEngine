#include <stdio.h>
#include <stdlib.h>
#include "htable.h"

struct htablerec {
    int capacity;
};

/**
 * initialises a new hashtable
 */
htable htable_new(int capacity) {
    
}

/**
 * FREEDOM
 */
void htable_free(htable h) {

}

/**
 * inserts into the hashtable with k = str, v = list
 */
int htable_put(htable h, char *str, int *list) {

}

/**
 * returns the associated docs
 */
int* htable_get(htable h, char *str) {

}
