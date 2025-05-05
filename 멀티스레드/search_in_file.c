#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "file_search.h"

void* worker_thread(void* arg) {
    thread_arg* t_arg = (thread_arg*) arg;
    char filepath[PATH_MAX];

    while (dequeue(t_arg->task_queue, filepath)) {
        FILE* fp = fopen(filepath, "r");
        if (!fp) {
            perror("파일 열기 실패");
            continue;
        }

        printf("[Thread %d] 검색 시작: %s\n", t_arg->thread_id, filepath);

        char line[1024];
        int line_num = 0;

        while (fgets(line, sizeof(line), fp)) {
            line_num++;
            if (strstr(line, t_arg->keyword)) {
                result_t res;
                strncpy(res.filepath, filepath, PATH_MAX);
                res.line_number = line_num;
                strncpy(res.line_content, line, sizeof(res.line_content));
                append_result(t_arg->result_list, &res);
            }
        }

        fclose(fp);
        printf("[Thread %d] 검색 종료: %s\n", t_arg->thread_id, filepath);
    }

    return NULL;
}