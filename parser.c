#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * Parses relevant data from a given xml file (wsj)
 */
void parser() {
    FILE *input = fopen("files/input/wsj.xml", "r");
    FILE *output = fopen("files/output/postparse.txt", "w+");
    if (input == NULL) {
        fprintf(stderr, "The required file is missing for parsing\n");
        exit(1);
    }
    int docCount = 0;
    char docNoTag[] = "<DOCNO>";
    // char *startTags[] = {"<HL>", "<IN>", "<TEXT>", "<DATELINE>", "<LP>"};
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
                // prints DOCNO
                while ((character = fgetc(input)) != '<') {
                    if (character == '\n') {
                        continue;
                    }
                    fprintf(output, "%c", character);
                }
                // skips over closing DOCNO tag
                while ((character = fgetc(input)) != '>') {
                    character = fgetc(input);
                }
                int endDoc = 0;
                while (1) {
                    if (character == '<') {
                        // skips over opening tag
                        while ((character = fgetc(input)) != '>') {
                            if (character == '/') {
                                // has to be </doc>
                                endDoc = 1;
                                break;
                            }
                        }
                        if (endDoc == 1) {
                            break;
                        }
                        // prints content of tag
                        while ((character = fgetc(input)) != '<') {
                            if (character != '\n') {
                                fprintf(output, "%c", tolower(character));
                            } else {
                                fprintf(output, " ");
                            }
                        }
                        // skips over opening tag
                        while ((character = fgetc(input)) != '>') {
                            ;
                        }
                    }
                    character = fgetc(input);
                }
                fprintf(output, "\n\n");
            }
        }
    }
    fclose(input);
    fclose(output);
}

void parserRewrite() {
    FILE *input = fopen("files/input/wsj.xml", "r");
    FILE *output = fopen("files/output/postparseRewrite.txt", "w+");
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
                fprintf(output, "%s ", token);
            } else if (strcmp(token, "<DOCNO") == 0) {
                fprintf(output, "%d %s ", docCount++, (token = strtok(NULL, " \n")));
            } else if (strcmp(token, "<DOC") == 0) {
                fprintf(output, "\n");
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
    FILE *input = fopen("files/output/postparse.txt", "r");
    FILE *midput = fopen("files/output/midclean.txt", "w+");
    FILE *output = fopen("files/output/postclean.txt", "w+");
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

void rmFiles() {
    int r;
    r = remove("files/output/postparse.txt");
    r = remove("files/output/midclean.txt");
}

int main() {
    parserRewrite();
    //cleanInput();
    //rmFiles();

    return EXIT_SUCCESS;
}