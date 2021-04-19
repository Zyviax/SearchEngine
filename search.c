#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *dict = fopen("test/test-dict.txt", "r");
    FILE *postings = fopen("test/test-postings.txt", "r");
    FILE *map = fopen("test/test-map.txt", "r");

    char query[100]; //assumption that there wont be a query longer than 100 characters
    int defaultSize = 5;
    int currSize = defaultSize;
    char **searchTerms = malloc(sizeof(char*) * defaultSize);

    fgets(query, 100, stdin);
    printf("Your query is: %s\n", query);
    char *token = strtok(query, " ");
    int searchTermsSize = 0;
    while (token != NULL) {
        if (searchTermsSize == currSize) {
            searchTerms = realloc(searchTerms, sizeof(char*) * currSize * 2);
            currSize = currSize * 2;
        }
        searchTerms[searchTermsSize] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(searchTerms[searchTermsSize], token);
        searchTermsSize++;
        token = strtok(NULL, " ");
    }

    /* TODO implement hashtable with <k,v> as <word, postings>
     * https://stackoverflow.com/questions/7666509/hash-function-for-string
     * http://www.cse.yorku.ca/~oz/hash.html
     */

    printf("You have %d search terms.\n", searchTermsSize);
    printf("The search terms are:\n");
    for (int i = 0; i < searchTermsSize; i++) {
        printf("%d %s\n", i, searchTerms[i]);
        free(searchTerms[i]);
    }

    printf("All search terms freed\n");
    free(searchTerms);
    printf("Query freed");
}
