#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "file_search.h"

void traverse_directory_to_queue(const char* path, task_queue_t* queue, const char* ext_filter, int depth, int max_depth) {
    if (depth > max_depth) return;

    DIR* dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "[경고] 디렉토리 열기 실패: %s (%s)\n", path, strerror(errno));
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (lstat(fullpath, &statbuf) == -1) continue;

        if (S_ISDIR(statbuf.st_mode)) {
            traverse_directory_to_queue(fullpath, queue, ext_filter, depth + 1, max_depth);
        } else if (S_ISREG(statbuf.st_mode)) {
            if (!ext_filter || strstr(entry->d_name, ext_filter)) {
                enqueue(queue, fullpath);
            }
        }
    }

    closedir(dir);
}
