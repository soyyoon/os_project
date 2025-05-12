#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_search.h"

void append_result(result_list_t* list, result_t* new_result) {
    result_node_t* new_node = malloc(sizeof(result_node_t));
    new_node->result = *new_result;
    new_node->next = NULL;

    if (!list->head) {
        list->head = list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
}

void save_results_to_csv(const char* filename, result_list_t* list) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("CSV 파일 열기 실패");
        return;
    }

    fprintf(fp, "File,Line,WordCount,Content\n");
    for (result_node_t* node = list->head; node; node = node->next) {
        fprintf(fp, "\"%s\",%d,%d,\"%s\"\n",
            node->result.filepath,
            node->result.line_number,
            node->result.word_count,
            node->result.line_content);
    }

    fclose(fp);
}
