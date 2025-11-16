
#include "arraylist.h"
#include "main_util.h"
#include <stdlib.h>
#include <string.h>

Arraylist *arraylist_create(size_t item_size, size_t initial_capacity){
    Arraylist *list = malloc(sizeof(Arraylist));
    if(list == NULL)
        ERROR_RETURN(NULL, "Could not allocate memory for Arraylist\n");

    list->item_size = item_size;
    list->capacity = initial_capacity;
    list->len = 0;
    list->items = malloc(item_size * initial_capacity);
    if(list->items==NULL)
        ERROR_RETURN(NULL, "Could not allocat memory for Arraylist-items\n");
    return list;
}

size_t arraylist_append(Arraylist *list, void *item){
    if(list->len == list->capacity){
        list->capacity = (list->capacity > 0) ? (list->capacity * 2) : 1;
        void *items = realloc(list->items, list->item_size * list->capacity);
        if(items==NULL)
            ERROR_RETURN(-1, "Could not append memory for Arraylist");

        list->items = items;
    }

    size_t index = list->len++;
    memcpy((uint8_t*)list->items + index*list->item_size, item, list->item_size);
    return index;
}

void *arraylist_get(Arraylist *list, size_t index){
    if(index>=list->len)
        ERROR_RETURN(NULL, "Index out of bounds\n");

    return (uint8_t*)list->items + index*list->item_size;
}

uint8_t arraylist_remove(Arraylist *list, size_t index){
    if(list->len==0)
        ERROR_RETURN(1, "list is empty\n");
    if(index>=list->len)
        ERROR_RETURN(1, "Index out of bounds\n");

    if(list->len==1){
        list->len =0;
        return 0;
    }
    
    --(list->len);
    
    uint8_t *item_ptr = (uint8_t*)list->items + index*list->item_size;
    uint8_t *end_ptr = (uint8_t*)list->items + list->len*list->item_size;
    memcpy(item_ptr, end_ptr, list->item_size);

    return 0;
}

void arraylist_free(Arraylist *list){
    if(list==NULL) {return;}
    if(list->items != NULL){
        free(list->items);
    }
    free(list);
}
