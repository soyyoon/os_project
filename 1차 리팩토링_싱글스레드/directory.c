#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "file_search.h"

void traverse_directory(const char* path, const char* keyword, result_list_t* list) {
    DIR* dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "디렉토리 열기 실패: %s (%s)\n", path, strerror(errno));
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (lstat(fullpath, &statbuf) == -1) {
            fprintf(stderr, "lstat 실패: %s (%s)\n", fullpath, strerror(errno));
            continue;
        }

        if (S_ISLNK(statbuf.st_mode)) {
            fprintf(stderr, "🔗 링크 무시: %s\n", fullpath);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            traverse_directory(fullpath, keyword, list);
        } else if (S_ISREG(statbuf.st_mode)) {
            pthread_t tid;
            thread_arg* t_arg = malloc(sizeof(thread_arg));
            strncpy(t_arg->filepath, fullpath, PATH_MAX);
            strncpy(t_arg->keyword, keyword, 256);
            t_arg->thread_id = rand() % 10000;
            t_arg->result_list = list;

            pthread_create(&tid, NULL, search_in_file, t_arg);
            pthread_detach(tid);
        }
    }

    closedir(dir);
}