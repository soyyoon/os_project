#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "file_search.h"

void traverse_directory_to_queue(const char* path, task_queue_t* queue) {
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

        if (S_ISLNK(statbuf.st_mode)) continue;

        if (S_ISDIR(statbuf.st_mode)) {
            traverse_directory_to_queue(fullpath, queue);
        } else if (S_ISREG(statbuf.st_mode)) {
            enqueue(queue, fullpath);
        }
    }

    closedir(dir);
}