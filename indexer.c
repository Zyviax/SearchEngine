#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "htable.h"

/**
 * Creates a dictionary.
 */
void createDict() {
    FILE *input = fopen("postclean.txt", "r");
    FILE *output = fopen("wordList.txt", "w");
    FILE *postings = fopen("postings.txt", "w");
    FILE *ids = fopen("docIDs.txt", "w");
    if (input == NULL) {
        fprintf(stderr, "The required file is missing for dictionary creation\n");
        exit(1);
    }

    int currId;
    char buffer[1600000];
    htable wordList = htable_new(1000000);
    int insertCount = 0;

    while (fgets(buffer, sizeof buffer, input) != NULL) {
        int tokenCount = 0;
        char *token = strtok(buffer, " \n");
        while (token != NULL) {
            if (tokenCount == 0) {
                currId = strtol(token, NULL, 10);
                tokenCount++;
            } else if (tokenCount == 1) {
                // prints docno to idfile
                fprintf(ids, "%s\n", token);
                tokenCount++;
            } else {
                int *temp = htable_get(wordList, token);
                if (temp == NULL) {
                    temp = malloc(sizeof(int));
                    temp[0] = currId;
                    // inserts into hashtable with a word and pointer containing the current docid val
                    if (3 > htable_put(wordList, token, temp, 1)) {
                        insertCount++;
                    }
                } else {
                    int size = htable_getSize(wordList, token);
                    // checks for duplicate docid val
                    if (temp[size-1] != currId) {
                        temp = realloc(temp, sizeof(int) * (size + 1));
                        temp[size] = currId;
                        htable_update(wordList, token, temp, size+1);
                    }
                }
            }
            token = strtok(NULL, " \n");
        }
        //prints current doc indexed with the number of unique words in the htable
        printf("%d %d\n", currId, insertCount); 
    }

    fclose(input);
    fclose(ids);
    printf("Dictionary created.\n");

    htable_print_keys(wordList, output);
    fclose(output);
    printf("wordList written to disk.\n");

    htable_print_vals(wordList, postings);
    fclose(postings);
    printf("postings written to disk.\n");

    htable_free(wordList);

    printf("Indexer finished.\n");
}

int main() {
    createDict();

    return EXIT_SUCCESS;
}
