// search.c (파일 내 키워드 검색 및 단어 수 세기)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include "file_search.h"

int count_words(const char* line) {
    int count = 0, in_word = 0;
    for (int i = 0; line[i]; i++) {
        if (isspace(line[i])) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
    }
    return count;
}

void* worker_thread(void* arg) {
    thread_arg* t_arg = (thread_arg*) arg;
    char filepath[PATH_MAX];

    while (dequeue(t_arg->task_queue, filepath)) {
        FILE* fp = fopen(filepath, "r");
        if (!fp) continue;

        printf("[Thread %d] 시작: %s\n", t_arg->thread_id, filepath);
        char line[MAX_LINE_LENGTH];
        int line_num = 0;

        while (fgets(line, sizeof(line), fp)) {
            line_num++;
            if (strstr(line, t_arg->keyword)) {
                result_t res;
                strncpy(res.filepath, filepath, PATH_MAX);
                res.line_number = line_num;
                strncpy(res.line_content, line, MAX_LINE_LENGTH);
                res.word_count = count_words(line);
                append_result(t_arg->result_list, &res);
            }
        }

        fclose(fp);
        printf("[Thread %d] 종료: %s\n", t_arg->thread_id, filepath);
    }

    return NULL;
}
