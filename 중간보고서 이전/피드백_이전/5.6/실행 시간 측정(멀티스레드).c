// 큐 기반 다중 스레드 검색 실행

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>   // ✅ 시간 측정용
#include "file_search.h"

#define NUM_THREADS 4

int main() {
    char keyword[256];
    printf("🔍 검색할 키워드를 입력하세요: ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) {
        fprintf(stderr, "키워드 입력 실패\n");
        return 1;
    }
    keyword[strcspn(keyword, "\n")] = '\0';

    result_list_t result_list = {0};
    task_queue_t task_queue;
    init_queue(&task_queue);

    const char* root_dir = "./test_root";

    // ✅ 시간 측정 시작
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    traverse_directory_to_queue(root_dir, &task_queue);

    pthread_t threads[NUM_THREADS];
    thread_arg args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        strncpy(args[i].keyword, keyword, 256);
        args[i].task_queue = &task_queue;
        args[i].result_list = &result_list;
        args[i].thread_id = i;
        pthread_create(&threads[i], NULL, worker_thread, &args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // ✅ 시간 측정 종료
    gettimeofday(&end_time, NULL);

    printf("\n=== 검색 결과 ===\n");
    result_node_t* node = result_list.head;
    while (node) {
        printf("파일: %s (줄 %d): %s", node->result.filepath, node->result.line_number, node->result.line_content);
        node = node->next;
    }

    // ✅ 시간 출력
    double elapsed_sec = (end_time.tv_sec - start_time.tv_sec) +
                         (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("\n⏱️ 실행 시간: %.6f초\n", elapsed_sec);

    return 0;
}
