#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *input = fopen("wsj.small.xml", "r");
    FILE *output = fopen("output.txt", "w");
    int docCount = 0;

    // char tagStart1[] = "<DOC>";
    // char tagEnd1[] = "</DOC>";
    char tagStart2[] = "<DOCNO>";
    char tagEnd2[] = "</DOCNO>";
    char tagStart3[] = "<IN>";
    char tagEnd3[] = "</TEXT>";

    char character;
    while ((character = fgetc(input)) != EOF) {
        int valid = 0; // if valid then 0 else 1;
        if (character == '<') {
            int tagIndex = 1;
            while (tagStart2[tagIndex] != '\0') {
                if ((character = fgetc(input)) != tagStart2[tagIndex]) {
                    valid = 1;
                    break;
                }
                tagIndex++;
            }
            if (valid == 0) {
                printf("%d", docCount++);
                while ((character = fgetc(input)) != '<') {
                    printf("%c", character);
                }
                printf("\n");
            }
        }
    }
}