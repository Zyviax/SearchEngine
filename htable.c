#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int *listSizes;
};

/**
 * initialises a new hashtable
 */
htable htable_new(int capacity) {
    htable h = malloc(sizeof *h);
    h->capacity = capacity;
    h->words = malloc(sizeof h->words[0] * capacity);
    h->lists = malloc(sizeof h->lists[0] * capacity);
    h->listSizes = malloc(sizeof(int) * capacity);
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
int htable_put(htable h, char *str, int *list, int size) {
    int key = hash(str)%h->capacity;
    int step = htable_step(h, hash(str));

    if (h->words[key] == NULL) {
        h->words[key] = malloc((strlen(str)+1) * sizeof str[0]);
        strcpy(h->words[key], str);
        h->lists[key] = list; // does this work?
        h->listSizes[key] = size;
        return 1;
    } else if (strcmp(str, h->words[key]) == 0) {
        // for now just do nothing
        return 3;
    } else { // collision resolution using double hashing
        int key_i;
        for (int i = 0; i < h->capacity; i++) {
            key_i = (key + i*step)%h->capacity;
            if (h->words[key_i] == NULL) {
                h->words[key_i] = malloc((strlen(str)+1) * sizeof str[0]);
                strcpy(h->words[key_i], str);
                h->lists[key_i] = list; // does this work?
                h->listSizes[key_i] = size;
                return 2;
            } else if (strcmp(str, h->words[key_i]) == 0) {
                // for now just do nothing
                return 3;
            }
        }
    }
    return 0;
}

// void htable_update(htable h, char *str, int *list, int newSize) {
//     int key = hash(str)%h->capacity;
//     int step = htable_step(h, hash(str));

//     if (h->words[key] == NULL) {
//         ; // shouldn't ever happen
//     } else if (strcmp(str, h->words[key]) == 0) {
//         // pointer gets updated in main program, only need to update the size
//         h->lists[key] = list;
//         h->listSizes[key] = newSize;
//     } else { // collision resolution using double hashing
//         int key_i;
//         for (int i = 0; i < h->capacity; i++) {
//             key_i = (key + i*step)%h->capacity;
//             if (h->words[key_i] == NULL) {
//                 ; // shouldn't ever happen
//                 if (strcmp(str, "6.8") == 0) {
//                     printf("crash<3.6> %s\n", str);
//                 }
//             } else if (strcmp(str, h->words[key_i]) == 0) {
//                 h->lists[key_i] = list;
//                 h->listSizes[key_i] = newSize;
//             }
//         }
//     }
// }

void htable_update(htable h, char *str, int *list, int newSize) {
    int key = hash(str)%h->capacity;
    int step = htable_step(h, hash(str));
    int index = key;
    
    for (int i = 0; i < h->capacity && h->words[index] != NULL; i++) {
        if (strcmp(str, h->words[index]) == 0) {
            h->lists[index] = list;
            h->listSizes[index] = newSize;
        } else {
            index = (key + i*step)%h->capacity;
        }
    }
}

/**
 * returns the associated docs, or NULL if str does not exist
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
    return NULL;
}

/**
 * returns the associated size, or -1 if str does not exist
 */
int htable_getSize(htable h, char *str) {
    int key = hash(str)%h->capacity;
    int step = htable_step(h, hash(str));
    int index = key;
    
    for (int i = 0; i < h->capacity && h->words[index] != NULL; i++) {
        if (strcmp(str, h->words[index]) == 0) {
            return h->listSizes[index];
        } else {
            index = (key + i*step)%h->capacity;
        }
    }
    return -1;
}

/**
 * prints all keys and values in the form:
 * k v1 v2 ... vn
 */
void htable_print(htable h) {
    for (int i = 0; i < h->capacity; i++) {
        if (h->words[i] != NULL) {
            printf("%s ", h->words[i]);
            for (int j = 0; j < h->listSizes[i]; j++) {
                printf("%d ", h->lists[i][j]);
            }
            printf("\n");
        } else {
            printf("NULL \n");
        }
    }
}

/**
 * prints all keys
 */
void htable_print_keys(htable h, FILE *stream) {
    for (int i = 0; i < h->capacity; i++) {
        if (h->words[i] != NULL) {
            fprintf(stream, "%s\n", h->words[i]);
        }
    }
}

/**
 * prints all lists
 */
void htable_print_vals(htable h, FILE *stream) {
    for (int i = 0; i < h->capacity; i++) {
        if (h->words[i] != NULL) {
            for (int j = 0; j < h->listSizes[i]; j++) {
                fprintf(stream, "%d ", h->lists[i][j]);
            }
            fprintf(stream, "\n");
        }
    }
}
