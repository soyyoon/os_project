//폴더만드는법을모르겠어요ㅠ

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

// 결과 노드 구조체
typedef struct result_node {
    result_t result;
    struct result_node* next;
} result_node_t;

// 공유 리스트 구조체
typedef struct {
    result_node_t* head;
    result_node_t* tail;
} result_list_t;

// 스레드 인자 구조체
typedef struct {
    char filepath[PATH_MAX];
    char keyword[256];
    int thread_id;
    result_list_t* result_list;
} thread_arg;

// 결과 추가 함수 (동기화 없음)
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

// ✅ 단일 스레드 처리 + 상태 로그 출력 포함
void* search_in_file(void* arg) {
    thread_arg* t_arg = (thread_arg*) arg;

    // 🔵 스레드 시작 로그
    printf("[Thread %d] 시작: %s\n", t_arg->thread_id, t_arg->filepath);

    FILE* fp = fopen(t_arg->filepath, "r");
    if (!fp) {
        perror("파일 열기 실패");
        printf("[Thread %d] 종료 (파일 열기 실패): %s\n", t_arg->thread_id, t_arg->filepath);
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

    // 🔵 스레드 종료 로그
    printf("[Thread %d] 종료: %s\n", t_arg->thread_id, t_arg->filepath);

    free(t_arg);
    return NULL;
}
//파일 하나당 하나의 스레드
//동기화없이구현
//실행 시작, 종료 시점 출력을 통해 병렬성 테스트에 활용 가능
//각 스레드가 상태 로그 독립적으로 출력
