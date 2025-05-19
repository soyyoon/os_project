// file_search.h (공통 구조체 및 함수 선언)
#ifndef FILE_SEARCH_H
#define FILE_SEARCH_H

#include <limits.h>
#include <stdbool.h>

#define PATH_MAX 1024
#define MAX_QUEUE_SIZE 1024
#define MAX_LINE_LENGTH 1024

// 검색 결과 저장 구조체
typedef struct {
    char filepath[PATH_MAX];
    int line_number;
    char line_content[MAX_LINE_LENGTH];
    int word_count;
} result_t;

// 결과 리스트 노드
typedef struct result_node {
    result_t result;
    struct result_node* next;
} result_node_t;

// 결과 리스트
typedef struct {
    result_node_t* head;
    result_node_t* tail;
} result_list_t;

// 작업 큐 구조체
typedef struct {
    char tasks[MAX_QUEUE_SIZE][PATH_MAX];
    int f;
    int r;
    int size;
} task_queue_t;

// 스레드 인자 구조체
typedef struct {
    char keyword[256];
    task_queue_t* task_queue;
    result_list_t* result_list;
    int thread_id;
} thread_arg;

// 함수 선언
void init_queue(task_queue_t* queue);
bool enqueue(task_queue_t* queue, const char* filepath);
bool dequeue(task_queue_t* queue, char* out_filepath);

void* worker_thread(void* arg);
void traverse_directory_to_queue(const char* path, task_queue_t* queue, const char* ext_filter, int depth, int max_depth);
void append_result(result_list_t* list, result_t* new_result);
void save_results_to_csv(const char* filename, result_list_t* list);
int count_words(const char* line);

#endif
