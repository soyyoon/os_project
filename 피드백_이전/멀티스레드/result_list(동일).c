#include <stdlib.h>
#include "file_search.h"

void append_result(result_list_t* list, result_t* new_result) {
    result_node_t* new_node = malloc(sizeof(result_node_t));
    new_node->result = *new_result;
    new_node->next = NULL;

    if (list->head == NULL) {
        list->head = list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
}
