#ifndef stegy_charlist_h
#define stegy_charlist_h

#include "common.h"

typedef struct {
    int count;
    int capacity;
    char* chars;
} CharList;

void initCharList(CharList* list);
void freeCharList(CharList* list);
void addToCharList(CharList* list, char element);

void copyConstantCharArray(CharList* list, const char* arr);

#endif // stegy_bytelist_h
