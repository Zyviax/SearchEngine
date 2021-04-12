#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * Parses relevant data from a given xml file (wsj)
 */
void parser() {
    FILE *input = fopen("wsj.xml", "r");
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
                fprintf(output, "%d ", docCount++);
                while ((character = fgetc(input)) != '<') {
                    if (character == '\n') {
                        continue;
                    }
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
    FILE *midput = fopen("midclean.txt", "w");
    FILE *output = fopen("postclean.txt", "w");
    char prevChar = ' ';
    char currChar = ' ';
    int unprinted = 0;
    int spaces = 0;
    char illegalChars[] = "`~!@#$%%^&*()-_=+[]{};:\"<>/?";
    int illegal;
    while ((currChar = fgetc(input)) != EOF) {
        illegal = 1;
        for (int i = 0; i < strlen(illegalChars); i++) {
            if (currChar == illegalChars[i]) {
                illegal = 0;
                break;
            }
        }
        if (currChar == '\n') {
            spaces = 0;
        } 
        if (spaces < 2) {
            if (currChar == ' ') {
                if (prevChar == ' ') {
                    continue;
                }
                spaces++;
            }
            fprintf(midput, "%c", currChar);
        } else if (illegal == 0) {
            fprintf(midput, " ");
            unprinted = 0;
        } else if (currChar == ' ') {
            if (prevChar == ',' || prevChar == '.') {
                unprinted = 0;
            }
            fprintf(midput, " ");
        } else if (currChar == ',' || currChar == '.') {
            if (prevChar == ',' || prevChar == '.') {
                fprintf(midput, " ");
                unprinted = 0;
            } else if (prevChar == ' ') {
                fprintf(midput, " ");
            } else {
                unprinted = 1;
            }
        } else if (currChar == '\'') {
            continue;
        } else {
            if (unprinted == 1) {
                fprintf(midput, "%c", prevChar);
                unprinted = 0;
            }
            fprintf(midput, "%c", currChar);
        }
        prevChar = currChar;
    }
    fclose(input);
    fclose(midput);

    midput = fopen("midclean.txt", "r");
    prevChar = '\0';
    currChar = '\0';
    while ((currChar = fgetc(midput)) != EOF) {
        if (currChar == ' ') {
            ;
        } else if (prevChar == ' ') {
            fprintf(output, " %c", currChar);
        } else {
            fprintf(output, "%c", currChar);
        }
        prevChar = currChar;
    }

    fclose(midput);
    fclose(output);
}

int main() {
    parser();
    cleanInput();
}