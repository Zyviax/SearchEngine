#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

int compareStrings(void *str1, void *str2) {

}

/**
 * Partially creates an inverted file
 */
void createDict() {
    FILE *input = fopen("postclean.txt", "r");
    FILE *output = fopen("wordlist.txt", "w");
    FILE *idMap = fopen("map.txt", "w");
    const int defaultSize = 5;
    char **wordList = malloc(sizeof(char*) * defaultSize);
    int currSize = defaultSize;
    int wordCount = 0;
    char curr = ' ';
    int currId = -1;

    while ((curr = fgetc(input)) != EOF) {
        if (currId == -1) {
            currId = 0;
            curr = fgetc(input);
            curr = fgetc(input);
            char *docNo = getNextWord(input, curr);
            fprintf(idMap, "%d %s\n", currId, docNo);
            free(docNo);
        } else if (curr == '\n') {
            //fprintf(output, "\n");
            while (curr == '\n') {
                curr = fgetc(input);
            }
            if (isdigit(curr)) {
                currId = curr - '0';
                curr = fgetc(input);
                curr = fgetc(input);
                char *docNo = getNextWord(input, curr);
                fprintf(idMap, "%d %s\n", currId, docNo);
                free(docNo);
            }
        } else if (curr != ' ') {
            char *word = getNextWord(input, curr);
            if (wordCount == 0) {
                wordList[0] = malloc(sizeof(word)+1);
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
                    wordList[wordCount] = malloc(sizeof(word)+1);
                    strcpy(wordList[wordCount], word);
                    wordCount++;
                }
            }
            if (wordCount == currSize) {
                wordList = realloc(wordList, currSize * sizeof(char*) * 2);
                currSize = currSize * 2;
            }
            //fprintf(output, "%s %d\n", word, currId);
            free(word);
        }
    }

    //qsort(wordList, wordCount, 20, strcmp);
    for (int i = 0; i < wordCount; i++) {
        fprintf(output, "%s\n", wordList[i]);
        free(wordList[i]);
    }
    free(wordList);

    fclose(input);
    fclose(output);
    fclose(idMap);
}

int main() {
    createDict();
}