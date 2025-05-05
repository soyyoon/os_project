#ifndef FILE_SEARCH_H
#define FILE_SEARCH_H

#include <limits.h>
#include <stdbool.h>

#define PATH_MAX 1024
#define MAX_QUEUE_SIZE 1024

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
} file_task_t;

typedef struct {
    file_task_t tasks[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} task_queue_t;

typedef struct {
    char keyword[256];
    task_queue_t* task_queue;
    result_list_t* result_list;
    int thread_id;
} thread_arg;

void init_queue(task_queue_t* queue);
bool enqueue(task_queue_t* queue, const char* filepath);
bool dequeue(task_queue_t* queue, char* out_filepath);

void* worker_thread(void* arg);
void traverse_directory_to_queue(const char* path, task_queue_t* queue);
void append_result(result_list_t* list, result_t* new_result);

#endif