#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "htable.h"

char *getNextWord(FILE *input, char curr) {
    const int defaultSize = 5;
    char *word = malloc(defaultSize * sizeof(char));
    int currSize = defaultSize;
    int wordIndex = 0;
    for (; curr != ' ' && curr != EOF; wordIndex++, curr = fgetc(input)) {
        word[wordIndex] = curr;
        if (currSize-1 == wordIndex) {
            word = realloc(word, currSize * sizeof(char) * 2);
            currSize = currSize * 2;
        }
    }
    word[wordIndex] = '\0';
    return word;
}

/**
 * some sort for now
 */
char **stringSort(char **wordList, int wordCount) {
    char *temp;
    for (int i = 0; i < wordCount; i++) {
        for (int j = 0; j+1 < wordCount; j++) {
            if (strcmp(wordList[j], wordList[j+1]) > 0) {
                temp = wordList[j];
                wordList[j] = wordList[j+1];
                wordList[j+1] = temp;
            }
        }
    }
    return wordList;
}

/**
 * Creates a dictionary.
 */
int createDict() {
    FILE *input = fopen("files/output/postclean.txt", "r");
    FILE *output = fopen("files/output/wordList.txt", "w+");
    FILE *postings = fopen("files/output/postings.txt", "w+");
    FILE *ids = fopen("files/output/docIDs.txt", "w+");
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
            //printf("crash<3> %s\n", token); //crashes on token = "6.8" for whatever reason
            // if (strcmp(token, "6.8") == 0) {
            //     printf("crash<3> %s %d\n", token, tokenCount);
            // }
            if (tokenCount == 0) {
                currId = strtol(token, NULL, 10);
                tokenCount++;
            } else if (tokenCount == 1) {
                fprintf(ids, "%s\n", token);
                tokenCount++;
            } else {
                // if (strcmp(token, "6.8") == 0) {
                //     printf("crash<3.1> %s\n", token);
                // }
                int *temp = htable_get(wordList, token);
                if (temp == NULL) {
                    temp = malloc(sizeof(int));
                    temp[0] = currId;
                    // if (strcmp(token, "6.8") == 0) {
                    //     printf("crash<4> %s\n", token);
                    // }
                    if (3 > htable_put(wordList, token, temp, 1)) {
                        insertCount++;
                    }
                    // if (strcmp(token, "6.8") == 0) {
                    //     printf("crash<5> %s\n", token);
                    // }
                } else {
                    int size = htable_getSize(wordList, token);
                    if (temp[size-1] != currId) {
                        //printf("tf: %d %d\n", temp[size-1], currId);
                        temp = realloc(temp, sizeof(int) * (size + 1));
                        temp[size] = currId;
                        // if (strcmp(token, "6.8") == 0) {
                        //     printf("crash<3.5> %s\n", token);
                        // }
                        htable_update(wordList, token, temp, size+1);

                        // int *newTemp = htable_get(wordList, token);
                        // printf("%s ", token);
                        // for (int s = 0; s < htable_getSize(wordList, token); s++) {
                        //     printf("%d ", newTemp[s]);
                        // }
                        // printf("\n");
                        //printf("crash<1>\n");
                    }
                }
            }
            // if (strcmp(token, "6.8") == 0) {
            //     printf("crash<7!> %s\n", token);
            // }
            token = strtok(NULL, " \n");
        }
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
    return 0;
}

// void createPostings(int wordCount) {
//     int *postingsList[wordCount]; //????
// }

int main() {
    int wordCount = createDict();
    //createPostings(wordCount);
}