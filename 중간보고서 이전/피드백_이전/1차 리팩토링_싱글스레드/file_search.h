#ifndef FILE_SEARCH_H
#define FILE_SEARCH_H

#include <limits.h>
#define PATH_MAX 1024

typedef struct {
    char filepath[PATH_MAX];
    int line_number;
    char line_content[1024];
} result_t;

typedef struct result_node {
    result_t result;
    struct result_node* next;
} result_node_t;

typedef struct {
    result_node_t* head;
    result_node_t* tail;
} result_list_t;

typedef struct {
    char filepath[PATH_MAX];
    char keyword[256];
    int thread_id;
    result_list_t* result_list;
} thread_arg;

// 함수 선언
void traverse_directory(const char* path, const char* keyword, result_list_t* list);
void* search_in_file(void* arg);
void append_result(result_list_t* list, result_t* new_result);

#endif