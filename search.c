#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"

int main() {
    FILE *dict = fopen("test/test-dict.txt", "r");
    FILE *postings = fopen("test/test-postings.txt", "r");
    FILE *map = fopen("test/test-map.txt", "r");

    char query[100]; //assumption that there wont be a query longer than 100 characters
    int defaultSize = 5;
    int searchTermsSize = defaultSize;
    char **searchTerms = malloc(sizeof(char*) * defaultSize);

    fgets(query, 100, stdin);
    printf("Your query is: %s\n", query);
    char *token = strtok(query, " ");
    int searchTermsIndex = 0;
    while (token != NULL) {
        if (searchTermsIndex == searchTermsSize) {
            searchTerms = realloc(searchTerms, sizeof(char*) * searchTermsSize * 2);
            searchTermsSize = searchTermsSize * 2;
        }
        searchTerms[searchTermsIndex] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(searchTerms[searchTermsIndex], token);
        searchTermsIndex++;
        token = strtok(NULL, " ");
    }

    /* TODO implement hashtable with <k,v> as <word, postings>
     * https://stackoverflow.com/questions/7666509/hash-function-for-string
     * http://www.cse.yorku.ca/~oz/hash.html
     */
    // htable wordList = htable_new(10);
    // char word[20];
    // char wordPostings[20];
    // int *wordPostingsAsList = malloc(sizeof(int) * 10);
    // while (fgets(word, 20, dict) != NULL) {
    //     fgets(wordPostings, 20, postings);
    //     token = strtok(wordPostings, " ");
    //     int i = 0;
    //     for (; token != NULL; i++) {
    //         wordPostingsAsList[i] = strtol(token, 0, 40000);
    //         token = strtok(NULL, " ");
    //     }
    //     wordPostingsAsList[i] = -1;
    //     htable_put(wordList, word, wordPostingsAsList);
    // }

    // int postingsListSize = defaultSize;
    // int *postingsList = malloc(sizeof(int) * postingsListSize);


    printf("You have %d search terms.\n", searchTermsIndex);
    printf("The search terms are:\n");
    for (int i = 0; i < searchTermsIndex; i++) {
        printf("%d %s\n", i, searchTerms[i]);
        free(searchTerms[i]);
    }

    printf("All search terms freed\n");
    free(searchTerms);
    printf("Query freed");
}
