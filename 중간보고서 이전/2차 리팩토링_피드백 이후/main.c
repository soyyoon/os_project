// main.c  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "file_search.h"

#define MAX_THREADS 4

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <keyword> [--ext=.txt] [--max-depth=N]\n", argv[0]);
        return 1;
    }

    const char* keyword = argv[1];
    const char* root_dir = "./test_root";
    const char* ext_filter = NULL;
    int max_depth = 10;

    // 인자 파싱
    for (int i = 2; i < argc; i++) {
        if (strncmp(argv[i], "--ext=", 6) == 0) {
            ext_filter = argv[i] + 6;
        } else if (strncmp(argv[i], "--max-depth=", 12) == 0) {
            max_depth = atoi(argv[i] + 12);
        }
    }

    task_queue_t queue;
    init_queue(&queue);

    result_list_t result_list = {0};

    traverse_directory_to_queue(root_dir, &queue, ext_filter, 0, max_depth);

    pthread_t tids[MAX_THREADS];
    thread_arg args[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) {
        strncpy(args[i].keyword, keyword, 256);
        args[i].task_queue = &queue;
        args[i].result_list = &result_list;
        args[i].thread_id = i;
        pthread_create(&tids[i], NULL, worker_thread, &args[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }

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
