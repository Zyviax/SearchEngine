#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

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
        if (i % 1000 == 0) {
            printf("%f%% sorted.\n", (double)i/(double)wordCount * 100);
        }
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
    FILE *input = fopen("postclean.txt", "r");
    FILE *preoutput = fopen("wordList_(unsorted).txt", "w");
    FILE *output = fopen("wordlist.txt", "w");
    FILE *idMap = fopen("map.txt", "w");
    const int defaultSize = 5;
    char **wordList = malloc(sizeof(char*) * defaultSize);
    int currSize = defaultSize;
    int wordCount = 0;
    int allWordsCount = 0;
    char curr = ' ';
    int currId = -1;

    clock_t start = clock();

    while ((curr = fgetc(input)) != EOF) {
        if (wordCount % 1000 == 0) {
            int msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
            printf("wordCount: %d, allWordsCount: %d, timeTaken: %d:%d:%d, currID: %d\n", wordCount, allWordsCount, msec/1000/60, (msec/1000)%60, msec%1000, currId);
        }
        // if (wordCount == 100000) {
        //     break;
        // }
        if (currId == -1) {
            currId = 0;
            curr = fgetc(input);
            curr = fgetc(input);
            char *docNo = getNextWord(input, curr);
            fprintf(idMap, "%d %s\n", currId, docNo);
            free(docNo);
        } else if (curr == '\n') {
            while (curr == '\n') {
                curr = fgetc(input);
            }
            if (isdigit(curr)) {
                currId = curr - '0';
                while ((curr = fgetc(input)) != ' ') {
                    currId = (currId * 10) + (curr - '0');
                }
                curr = fgetc(input);
                char *docNo = getNextWord(input, curr);
                fprintf(idMap, "%d %s\n", currId, docNo);
                free(docNo);
            }
        } else if (curr != ' ') {
            char *word = getNextWord(input, curr);
            if (wordCount == 0) {
                wordList[0] = malloc(strlen(word) * sizeof(char) + 1);
                strcpy(wordList[0], word);
                wordCount++;
            } else {
                int uniqueWord = 0;
                for (int i = 0; i < wordCount; i++) {
                    if (strcmp(wordList[i], word) == 0) {
                        uniqueWord = 1;
                        break;
                    }
                }
                if (uniqueWord == 0) {
                    wordList[wordCount] = malloc(strlen(word) * sizeof(char) + 1);
                    strcpy(wordList[wordCount], word);
                    wordCount++;
                }
            }
            if (wordCount == currSize) {
                wordList = realloc(wordList, currSize * sizeof(char*) * 2);
                currSize = currSize * 2;
            }
            free(word);
            allWordsCount++;
        }
    }
    fclose(input);
    fclose(idMap);
    printf("Dictionary created.\n");

    for (int i = 0; i < wordCount; i++) {
        fprintf(preoutput, "%s\n", wordList[i]);
    }
    fclose(preoutput);
    printf("Unsorted dictionary written to disk.\n");

    wordList = stringSort(wordList, wordCount);
    int msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Dictionary sorted. timeTaken: %d:%d:%d\n", msec/1000/60, (msec/1000)%60, msec%1000);

    for (int i = 0; i < wordCount; i++) {
        fprintf(output, "%s\n", wordList[i]);
    }
    fclose(output);
    printf("Sorted dictionary written to disk.\n");

    for (int i = 0; i < wordCount; i++) {
        free(wordList[i]);
    }
    printf("Dictionary entries freed.\n");

    free(wordList);
    printf("Dictionary freed.\n");

    printf("Indexer finished.\n");
    return wordCount;
}

void createPostings(int wordCount) {
    int *postingsList[wordCount]; //????
}

int main() {
    int wordCount = createDict();
    createPostings(wordCount);
}