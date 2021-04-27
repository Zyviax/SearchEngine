#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#define HTABLE_SIZE 10 // there are (376370) words in the wordList
#define DOC_MAX 5 //exact number of documents (173252)

struct docRsvPair {
    char docId[50];
    int rsv;
}docRsvPair;

/* TODO implement hashtable with <k,v> as <word, postings>
 * https://stackoverflow.com/questions/7666509/hash-function-for-string
 * http://www.cse.yorku.ca/~oz/hash.html
 */
htable loadTable(FILE *dict, FILE *postings) {
    int htable_size = 10;
    htable wordList = htable_new(htable_size);
    char word[20]; //assume no word greater than 20 chars
    char wordPostings[100000]; //enough room to store up to ~18000 ints on worst case.
    int *wordPostingsAsList;
    char *token;
    while (fgets(word, sizeof word, dict) != NULL) {
        word[strcspn(word, "\n")] = 0;
        wordPostingsAsList = malloc(sizeof(int) * 10);
        fgets(wordPostings, sizeof wordPostings, postings);
        token = strtok(wordPostings, " \n");
        int i = 0;
        for (; token != NULL; i++) {
            wordPostingsAsList[i] = strtol(token, NULL, 10);
            token = strtok(NULL, " \n");
        }
        int size = i;
        htable_put(wordList, word, wordPostingsAsList, size);
    }
    return wordList;
}

void **loadIDs(char **docIds, FILE *idFile) {
    char line[30];
    int docCount = 0;
    while (fgets(line, sizeof line, idFile) != NULL) {
        line[strcspn(line, "\n")] = 0;
        // printf("%d\n", (strlen(line)+1));
        docIds[docCount] = malloc(sizeof(char) * (strlen(line)+1));
        strcpy(docIds[docCount], line);
        // printf("line %d: [%s]\n", docCount, docIds[docCount]);
        docCount++;
    }
    // for (int i = 0; i < 5; i++) {
    //     printf("%s\n", docIds[i]);
    // }

}

void getRelevantDocs(int *allDocIDs, htable words, char **queryList, int querySize) {
    for (int i = 0; i < querySize; i++) {
        int *docIDs;
        if ((docIDs = htable_get(words, queryList[i])) != NULL) {
            for (int j = 0; j < htable_getSize(words, queryList[i]); j++) {
                allDocIDs[docIDs[j]]++;
            }
            //printf("\n");
        }
    }
}

int comparePairs(const void *s1, const void *s2) {
    const struct docRsvPair* const * d1 = s1;
    const struct docRsvPair* const * d2 = s2;

    // printf("%s %d\n", (*d1)->docId, (*d1)->rsv);
    // printf("%s %d\n", (*d2)->docId, (*d2)->rsv);
    //printf("%d %d %d\n", d1->rsv, d2->rsv,d1->rsv - d2->rsv);
    return -((*d1)->rsv - (*d2)->rsv);
}

int main() {
    FILE *dict = fopen("files/test/test-dict.txt", "r");
    FILE *postings = fopen("files/test/test-postings.txt", "r");
    FILE *map = fopen("files/test/test-map.txt", "r");
    if (dict == NULL || postings == NULL || map == NULL) {
        fprintf(stderr, "The required files are missing\n");
        exit(1);
    }
    htable wordList = loadTable(dict, postings);
    char **docIds = malloc(sizeof(char*) * DOC_MAX);
    loadIDs(docIds, map);

    // for (int i = 0; i < 5; i++) {
    //     printf("%s\n", docIds[i]);
    // }

    char query[100]; //assumption that there wont be a query longer than 100 characters
    int defaultSize = 5; //default size of array holding query terms
    int searchTermsSize = defaultSize;
    char **searchTerms = malloc(sizeof(char*) * defaultSize);

    printf("Please enter a query: ");
    fgets(query, sizeof query, stdin);
    //printf("Your query is: %s\n", query);
    char *token = strtok(query, " ");
    int searchTermsIndex = 0;
    while (token != NULL) {
        if (searchTermsIndex == searchTermsSize) {
            searchTerms = realloc(searchTerms, sizeof(char*) * searchTermsSize * 2);
            searchTermsSize = searchTermsSize * 2;
        }
        token[strcspn(token, "\n")] = 0;
        searchTerms[searchTermsIndex] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(searchTerms[searchTermsIndex], token);
        searchTermsIndex++;
        token = strtok(NULL, " ");
    }

    // printf("You have %d search terms.\n", searchTermsIndex);
    // printf("The search terms are:\n");
    // for (int i = 0; i < searchTermsIndex; i++) {
    //     printf("%d %s\n", i, searchTerms[i]);
    // }

    int relevantDocs[DOC_MAX] = {0};
    getRelevantDocs(relevantDocs, wordList, searchTerms, searchTermsIndex);
    int numRelevantDocs = 0;
    struct docRsvPair **docRsvPairs = malloc(sizeof(struct docRsvPair*) * DOC_MAX);
    for (int i = 0; i < DOC_MAX; i++) {
        if (relevantDocs[i] > 0) {
            // printf("%s <rsv>", "test");
            // printf("%s %d\n", docIds[i], relevantDocs[i]);
            docRsvPairs[numRelevantDocs] = malloc(sizeof(struct docRsvPair));
            strcpy(docRsvPairs[numRelevantDocs]->docId, docIds[i]);
            docRsvPairs[numRelevantDocs]->rsv = relevantDocs[i];
            numRelevantDocs++;
        }
    }


    //printf("Query has relevance in:\n");
    // for (int i = 0; i < numRelevantDocs; i++) {
    //     printf("%s %d\n", docRsvPairs[i]->docId, docRsvPairs[i]->rsv);
    // }
    // printf("\nsorting...\n");

    printf("%d", numRelevantDocs);
    qsort(docRsvPairs, numRelevantDocs, sizeof(struct docRsvPair*), comparePairs);
    //printf("after sort:\n");
    for (int i = 0; i < numRelevantDocs; i++) {
        printf("%s %d\n", docRsvPairs[i]->docId, docRsvPairs[i]->rsv);
    }

    // cleaning up
    //printf("\n");
    for (int i = 0; i < searchTermsIndex; i++) {
        free(searchTerms[i]);
    }
    //printf("All search terms freed\n");
    free(searchTerms);
    //printf("Query freed\n");
    htable_free(wordList);
    //printf("Dict freed\n");
    for (int i = 0; i < DOC_MAX; i++) {
        free(docIds[i]);
    }
    free(docIds);
    //printf("ids freed\n");
    for (int i = 0; i < numRelevantDocs; i++) {
        free(docRsvPairs[i]);
    }
    free(docRsvPairs);
    //printf("docRsvPairs freed\n");

    //printf("\n");
    fclose(dict);
    fclose(postings);
    fclose(map);
    //printf("Files closed\n");
    
    return EXIT_SUCCESS;
}
