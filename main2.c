#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "file_search.h"

#define MAX_THREADS    1
#define DEFAULT_PROCS  2

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <keyword> [--ext=.txt] [--max-depth=N] [--processes=M]\n", argv[0]);
        return 1;
    }

    const char* keyword    = argv[1];
    const char* root_dir   = "./test_root";
    const char* ext_filter = NULL;
    int         max_depth  = 10;
    int         num_procs  = DEFAULT_PROCS;

    // 인자 파싱
    for (int i = 2; i < argc; i++) {
        if (strncmp(argv[i], "--ext=", 6) == 0) {
            ext_filter = argv[i] + 6;
        } else if (strncmp(argv[i], "--max-depth=", 12) == 0) {
            max_depth = atoi(argv[i] + 12);
        } else if (strncmp(argv[i], "--processes=", 11) == 0) {
            num_procs = atoi(argv[i] + 11);
            if (num_procs < 1) num_procs = 1;
        }
    }

    // 작업 큐 초기화 및 파일 경로 수집
    task_queue_t queue;
    init_queue(&queue);
    result_list_t result_list = {0};
    traverse_directory_to_queue(root_dir, &queue, ext_filter, 0, max_depth);

    // 멀티프로세스 모드 (num_procs > 1)
    if (num_procs > 1) {
        for (int p = 0; p < num_procs; p++) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(1);
            }
            if (pid == 0) {
                // 자식 프로세스: 스레드 워커 실행
                pthread_t   tids[MAX_THREADS];
                thread_arg  args[MAX_THREADS];
                for (int t = 0; t < MAX_THREADS; t++) {
                    strncpy(args[t].keyword, keyword, 256);
                    args[t].task_queue   = &queue;
                    args[t].result_list  = &result_list;
                    args[t].thread_id    = t;
                    pthread_create(&tids[t], NULL, worker_thread, &args[t]);
                }
                for (int t = 0; t < MAX_THREADS; t++) {
                    pthread_join(tids[t], NULL);
                }
                // 결과 저장
                char filename[64];
                snprintf(filename, sizeof(filename), "results_%d.csv", p);
                save_results_to_csv(filename, &result_list);
                printf("Child %d: 결과가 %s에 저장되었습니다.\n", p, filename);
                exit(0);
            }
            // 부모: 다음 자식 계속 생성
        }
        // 부모: 모든 자식 대기
        for (int p = 0; p < num_procs; p++) {
            wait(NULL);
        }
        printf("모든 자식 프로세스 완료.\n");
        return 0;
    }

    // 단일 프로세스 모드 (스레드만)
    pthread_t  tids[MAX_THREADS];
    thread_arg args[MAX_THREADS];
    for (int t = 0; t < MAX_THREADS; t++) {
        strncpy(args[t].keyword, keyword, 256);
        args[t].task_queue   = &queue;
        args[t].result_list  = &result_list;
        args[t].thread_id    = t;
        pthread_create(&tids[t], NULL, worker_thread, &args[t]);
    }
    for (int t = 0; t < MAX_THREADS; t++) {
        pthread_join(tids[t], NULL);
    }

    // 결과 출력 및 저장
    result_node_t* node = result_list.head;
    while (node) {
        printf("파일: %s (줄 %d, %d단어) → %s",
               node->result.filepath,
               node->result.line_number,
               node->result.word_count,
               node->result.line_content);
        node = node->next;
    }
    save_results_to_csv("results.csv", &result_list);
    printf("\n결과가 results.csv에 저장되었습니다.\n");
    return 0;
}

