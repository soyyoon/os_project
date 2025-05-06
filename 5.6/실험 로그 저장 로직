//search_in_file() 에서 로그 저장 로직을 포함시킨 전체 코드

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

// 결과 구조체
typedef struct {
    char filepath[PATH_MAX];
    int line_number;
    char line_content[1024];
} result_t;

// 결과 노드
typedef struct result_node {
    result_t result;
    struct result_node* next;
} result_node_t;

// 결과 리스트
typedef struct {
    result_node_t* head;
    result_node_t* tail;
} result_list_t;

// 스레드 인자 구조체 (로그 파일 포함)
typedef struct {
    char filepath[PATH_MAX];
    char keyword[256];
    int thread_id;
    result_list_t* result_list;
    FILE* log_fp;  // 🔵 로그 저장용 파일 포인터
} thread_arg;

// 리스트에 결과 추가 (동기화 없이 단순 연결)
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

// 스레드 내부 작업 함수 + 로그 저장 포함
void* search_in_file(void* arg) {
    thread_arg* t_arg = (thread_arg*) arg;

    // 시작 로그
    printf("[Thread %d] 시작: %s\n", t_arg->thread_id, t_arg->filepath);
    if (t_arg->log_fp) {
        fprintf(t_arg->log_fp, "[Thread %d] 시작: %s\n", t_arg->thread_id, t_arg->filepath);
        fflush(t_arg->log_fp);
    }

    FILE* fp = fopen(t_arg->filepath, "r");
    if (!fp) {
        perror("파일 열기 실패");
        if (t_arg->log_fp) {
            fprintf(t_arg->log_fp, "[Thread %d] 종료 (파일 열기 실패): %s\n", t_arg->thread_id, t_arg->filepath);
            fflush(t_arg->log_fp);
        }
        free(t_arg);
        return NULL;
    }

    char line[1024];
    int line_num = 0;
    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        if (strstr(line, t_arg->keyword)) {
            result_t res;
            strncpy(res.filepath, t_arg->filepath, PATH_MAX);
            res.line_number = line_num;
            strncpy(res.line_content, line, sizeof(res.line_content));
            append_result(t_arg->result_list, &res);
        }
    }

    fclose(fp);

    // 종료 로그
    printf("[Thread %d] 종료: %s\n", t_arg->thread_id, t_arg->filepath);
    if (t_arg->log_fp) {
        fprintf(t_arg->log_fp, "[Thread %d] 종료: %s\n", t_arg->thread_id, t_arg->filepath);
        fflush(t_arg->log_fp);
    }

    free(t_arg);
    return NULL;
}

//log_fp는 main()에서 열고, 각 스레드에 인자로 전달
