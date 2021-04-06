#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Parses relevant data from a given xml file (wsj)
 */
void parser() {
    FILE *input = fopen("wsj.small.xml", "r");
    FILE *output = fopen("postparse.txt", "w");
    int docCount = 0;
    char docNoTag[] = "<DOCNO>";
    int startTagCount = 0;
    char *startTags[] = {"<HL>", "<IN>", "<TEXT>"};
    char character;
    while ((character = fgetc(input)) != EOF) {
        int validDocTag = 1; // if valid then 1 else 0;
        if (character == '<') {
            int tagIndex = 1;
            // checks if the following text matches "DOCNO>"
            while (docNoTag[tagIndex] != '\0') {
                if ((character = fgetc(input)) != docNoTag[tagIndex]) {
                    validDocTag = 0;
                    break;
                }
                tagIndex++;
            }
            if (validDocTag == 1) {
                fprintf(output, "%d", docCount++);
                while ((character = fgetc(input)) != '<') {
                    fprintf(output, "%c", character);
                }
                while (startTagCount < (sizeof(startTags)/sizeof(startTags[0]))) {
                    tagIndex = 1;
                    if (character == '<') {
                        int validTag = 1;
                        // checks if the following text matches "HL>", "IN>" or "TEXT>"
                        while (startTags[startTagCount][tagIndex] != '\0') {
                            if ((character = fgetc(input)) != startTags[startTagCount][tagIndex]) {
                                validTag = 0;
                                break;
                            }
                            tagIndex++;
                        }
                        if (validTag == 1) {
                            while ((character = fgetc(input)) != '<') {
                                if (character != '\n') {
                                    fprintf(output, "%c", tolower(character));
                                } else {
                                    fprintf(output, " ");
                                }
                            }
                            startTagCount++;
                        }
                    }
                    character = fgetc(input);
                }
                fprintf(output, "\n\n");
                startTagCount = 0;
            }
        }
    }
    fclose(input);
    fclose(output);
}

/**
 * Strips unneeded punctuation and spacing
 */
void cleanInput() {
    FILE *input = fopen("postparse.txt", "r");
    FILE *output = fopen("postclean.txt", "w");
    char prevChar = ' ';
    char currChar = ' ';
    int unprinted = 0;
    while ((currChar = fgetc(input)) != EOF) {
        if (currChar == '(' || currChar == ')' || currChar == '"' || currChar == '\'') {
            continue;
        } else if (currChar == ' ' || currChar == ',' || currChar == '.') {
            unprinted = 1;
        } else if (currChar == ' ') {
            if (prevChar == ' ') {
                continue;
            } else if (unprinted == 0) {
                fprintf(output, "%c", currChar);
                unprinted = 0;               
            } 
        } else {
            if (unprinted == 1) {
                fprintf(output, "%c", prevChar);
                unprinted = 0;
            }
            fprintf(output, "%c", currChar);
        }
        prevChar = currChar;
    }
    fclose(input);
    fclose(output);
}

/**
 * Creates an inverted file
 */
void indexer() {
    FILE *input = fopen("postclean.txt", "r");
    FILE *output = fopen("invertedfile.txt", "w");
    char curr = ' ';
    const int defaultSize = 5;

    while ((curr = fgetc(input)) != EOF) {
        if (curr == '\n') {
            fprintf(output, "\n");
            printf("testtstst");
        }
        if (curr != ' ') {
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
            fprintf(output, "%s\n", word);
            free(word);
        }
    }

    fclose(input);
    fclose(output);
}

int main() {
    //parser();
    //cleanInput();
    indexer();
}