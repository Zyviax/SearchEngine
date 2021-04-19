#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdio.h>

typedef struct htablerec *htable;

extern void htable_free(htable h);
extern int htable_put(htable h, char *str, int *docs);
extern htable htable_new(int capacity);
extern int* htable_get(htable h, char *str);

#endif
