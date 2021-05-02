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
    if (input == NULL) {
        fprintf(stderr, "The required file is missing for parsing\n");
        exit(1);
    }

    int docCount = 0;
    char buffer[1000];

    while (fgets(buffer, sizeof buffer, input) != NULL) {
        char *token = strtok(buffer, " >\n");
        while (token != NULL) {
            if (token[0] != '<') {
                for (int i = 0; i < strlen(token); i++) {
                    if (token[i] == '<') {
                        token[i] = '\0';
                        break;
                    }
                }
                fprintf(output, "%s ", token);
            } else if (strcmp(token, "<DOC") == 0) {
                if (docCount > 0) {
                    fprintf(output, "\n\n");
                }
                fprintf(output, "%d ", docCount);
                docCount++;
            }
            token = strtok(NULL, " >\n");
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
    FILE *midput = fopen("midclean.txt", "w+");
    FILE *output = fopen("postclean.txt", "w");
    if (input == NULL) {
        fprintf(stderr, "The required file is missing for cleaning\n");
        exit(1);
    }
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
                int prevIllegal = 1;
                for (int i = 0; i < strlen(illegalChars); i++) {
                    if (prevChar == illegalChars[i]) {
                        prevIllegal = 0;
                        break;
                    }
                }
                if (prevIllegal == 0) {
                    ;
                } else {
                    unprinted = 1;
                }
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
    rewind(midput);

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

/**
 * Removes unneeded files made during parsing.
 */
void rmFiles() {
    int r;
    r = remove("postparse.txt");
    r = remove("midclean.txt");
}

int main() {
    parser();
    cleanInput();
    rmFiles();

    return EXIT_SUCCESS;
}