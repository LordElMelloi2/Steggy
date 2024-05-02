#include "charlist.h"
#include "memory.h"

void initCharList(CharList* list){
    list->count = 0;
    list->capacity = 0;
    list->chars = NULL;
}

void freeCharList(CharList* list){
    FREE_ARRAY(char, list->chars, list->capacity);
    initCharList(list);
}

void addToCharList(CharList* list, char element){
    if(list->capacity < list->count + 1) {
        int oldCapacity = list->capacity;
        list->capacity = GROW_CAPACITY(list->capacity);
        list->chars = GROW_ARRAY(char, list->chars, oldCapacity, list->capacity);
    }

    list->chars[list->count] = element;
    list->count++;
}

void copyConstantCharArray(CharList* list, const char* arr){
    uint32_t idx = 0;
    while (arr[idx] != '\0') {
        addToCharList(list, arr[idx]);
        idx++;
    }
}
