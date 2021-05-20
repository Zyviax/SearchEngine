#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#define HTABLE_SIZE 10000000
#define DOC_MAX 173252 //exact number of documents
#define WORD_MAX 376370 //exact number of words in wordList

/**
 * struct for holding 
 * a docID and associated rsv value
 */
struct docRsvPair {
    char docId[50];
    int rsv;
}docRsvPair;

/**
 * Creates a hashtable with: 
 * key = word;
 * value = list with only 1 value, that being line number of the word/postings in the index
 */
htable loadTable(FILE *dict, FILE *postings) {
    htable wordList = htable_new(HTABLE_SIZE);
    char word[100]; //assume no word greater than 100 chars
    char *token;
    int *pointer;
    int wordID = 0;
    while (fgets(word, sizeof word, dict) != NULL) {
        word[strcspn(word, "\r\n")] = 0;
        pointer = malloc(sizeof(int));
        pointer[0] = wordID;
        htable_put(wordList, word, pointer, 1);
        wordID++;
    }
    return wordList;
}

/**
 * Loads DocIDs into memory, just stored in an array
 */
void **loadIDs(char **docIds, FILE *idFile) {
    char *line = malloc(sizeof(char) * 50);
    int docCount = 0;
    while (fgets(line, 50, idFile) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        docIds[docCount] = malloc(sizeof(char) * (strlen(line)+1));
        strcpy(docIds[docCount], line);
        docCount++;
    }
    free(line);
}

/**
 * Loads lines of postings values into memory, just stored in an array
 */
void **loadPostings(char **postingsArr, FILE *postings) {
    char *line = malloc(sizeof(char) * 10000000);
    int wordCount = 0;
    while (fgets(line, 10000000, postings) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        postingsArr[wordCount] = malloc(sizeof(char) * (strlen(line)+1));
        strcpy(postingsArr[wordCount], line);
        wordCount++;
    }
    free(line);
}

/**
 * Given a query finds the relevant all relevent docs, based on occurence of the query word in the doc
 */
void getRelevantDocs(int *allDocIDs, htable words, char **postingsArr, char **queryList, int querySize) {
    char *posting = malloc(sizeof(char) * 10000000);
    for (int i = 0; i < querySize; i++) {
        int *wordID;
        if ((wordID = htable_get(words, queryList[i])) != NULL) {
            strcpy(posting, postingsArr[wordID[0]]);
            char *token = strtok(posting, " ");
            while (token != NULL) {
                allDocIDs[strtol(token, NULL, 10)]++;
                token = strtok(NULL, " ");
            }
        }
    }
    free(posting);
}

/**
 * Compares the rsv values of docRsvPairs, sorts highest to lowest
 */
int comparePairs(const void *s1, const void *s2) {
    const struct docRsvPair* const * d1 = s1;
    const struct docRsvPair* const * d2 = s2;
    return -((*d1)->rsv - (*d2)->rsv);
}

int main() {
    FILE *dict = fopen("wordList.txt", "r");
    FILE *postings = fopen("postings.txt", "r");
    FILE *map = fopen("docIDs.txt", "r");
    if (dict == NULL || postings == NULL || map == NULL) {
        fprintf(stderr, "The required files are missing\n");
        exit(1);
    }
    htable wordList = loadTable(dict, postings);
    char **docIds = malloc(sizeof(char*) * DOC_MAX);
    loadIDs(docIds, map);
    char **postingsArr = malloc(sizeof(char*) * WORD_MAX);
    loadPostings(postingsArr, postings);

    char query[200]; //assumption that there wont be a query longer than 100 characters
    int defaultSize = 5; //default size of array holding query terms

    while (fgets(query, sizeof query, stdin) != NULL) {
        int searchTermsSize = defaultSize;
        char **searchTerms = malloc(sizeof(char*) * searchTermsSize);
        char *token = strtok(query, " ");
        int searchTermsIndex = 0;
        while (token != NULL) {
            if (searchTermsIndex == searchTermsSize) {
                searchTerms = realloc(searchTerms, sizeof(char*) * searchTermsSize * 2);
                searchTermsSize = searchTermsSize * 2;
            }
            token[strcspn(token, "\r\n")] = 0; //removes trailing \n
            searchTerms[searchTermsIndex] = malloc(sizeof(char) * (strlen(token) + 1));
            strcpy(searchTerms[searchTermsIndex], token);
            searchTermsIndex++;
            token = strtok(NULL, " ");
        }

        int relevantDocs[DOC_MAX] = {0};
        getRelevantDocs(relevantDocs, wordList, postingsArr, searchTerms, searchTermsIndex);

        int numRelevantDocs = 0;
        struct docRsvPair **docRsvPairs = malloc(sizeof(struct docRsvPair*) * DOC_MAX);
        for (int i = 0; i < DOC_MAX; i++) {
            if (relevantDocs[i] > 0) {
                docRsvPairs[numRelevantDocs] = malloc(sizeof(struct docRsvPair));
                strcpy(docRsvPairs[numRelevantDocs]->docId, docIds[i]);
                docRsvPairs[numRelevantDocs]->rsv = relevantDocs[i];
                numRelevantDocs++;
            }
        }

        qsort(docRsvPairs, numRelevantDocs, sizeof(struct docRsvPair*), comparePairs);
        for (int i = 0; i < numRelevantDocs; i++) {
            printf("%s %d\n", docRsvPairs[i]->docId, docRsvPairs[i]->rsv);
        }

        for (int i = 0; i < searchTermsIndex; i++) {
            free(searchTerms[i]);
        }
        free(searchTerms);
        for (int i = 0; i < numRelevantDocs; i++) {
            free(docRsvPairs[i]);
        }
        free(docRsvPairs);
    }

    htable_free(wordList);
    for (int i = 0; i < DOC_MAX; i++) {
        free(docIds[i]);
    }
    free(docIds);
    for (int i = 0; i < WORD_MAX; i++) {
        free(postingsArr[i]);
    }
    free(postingsArr);

    fclose(dict);
    fclose(postings);
    fclose(map);
    
    return EXIT_SUCCESS;
}
