#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdio.h>

typedef struct htablerec *htable;

extern void htable_free(htable h);
extern int htable_put(htable h, char *str, int *docs, int size);
extern void htable_update(htable h, char *str, int *list, int newSize);
extern htable htable_new(int capacity);
extern int* htable_get(htable h, char *str);
extern int htable_getSize(htable h, char *str);
extern void htable_print(htable h);
extern void htable_print_keys(htable h, FILE *stream);
extern void htable_print_vals(htable h, FILE *stream);

#endif
