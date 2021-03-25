#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *input = fopen("wsj.small.xml", "r");
    FILE *output = fopen("output.txt", "w");
    int docCount = 0;
    int startTagCount = 0;
    char docNoTag[] = "<DOCNO>";
    char *startTags[] = {"<HL>", "<IN>", "<TEXT>"};
    char character;
    while ((character = fgetc(input)) != EOF) {
        int validDocTag = 0; // if valid then 0 else 1;
        if (character == '<') {
            int tagIndex = 1;
            while (docNoTag[tagIndex] != '\0') {
                if ((character = fgetc(input)) != docNoTag[tagIndex]) {
                    validDocTag = 1;
                    break;
                }
                tagIndex++;
            }
            if (validDocTag == 0) {
                printf("%d", docCount++);
                while ((character = fgetc(input)) != '<') {
                    printf("%c", character);
                }

                printf("[");
                while (startTagCount < (sizeof(startTags)/sizeof(startTags[0]))) {
                    tagIndex = 1;
                    if (character == '<') {
                        int validTag = 0;
                        while (startTags[startTagCount][tagIndex] != '\0') {
                            if ((character = fgetc(input)) != startTags[startTagCount][tagIndex]) {
                                validTag = 1;
                                break;
                            }
                            tagIndex++;
                        }
                        if (validTag == 0) {
                            while ((character = fgetc(input)) != '<') {
                                if (character != '\n') {
                                    printf("%c", character);
                                }
                            }
                            startTagCount++;
                        }
                    }
                    character = fgetc(input);
                }
                printf("]\n\n");
                startTagCount = 0;
            }
        }
    }
}