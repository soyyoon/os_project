// [라] parallel_search.c — child + thread 혼합형 구조 (parent도 디렉토리 탐색 참여)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define KEYWORD "target"
#define ROOT_PATH "./dataset"
#define MAX_CHILDREN 4
#define MAX_THREADS 8
#define USE_MUTEX 1

pthread_mutex_t print_lock;

typedef struct {
    char path[PATH_MAX];
} thread_arg_t;

void search_in_file(const char* filepath) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) return;
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, KEYWORD)) {
            found = 1;
            break;
        }
    }
    fclose(fp);
#if USE_MUTEX
    pthread_mutex_lock(&print_lock);
#endif
    printf("[%d] %s: %s\n", getpid(), filepath, found ? "FOUND" : "NOT FOUND");
#if USE_MUTEX
    pthread_mutex_unlock(&print_lock);
#endif
}

void scan_directory(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) return;
    struct dirent* entry;
    char fullpath[PATH_MAX];
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        struct stat st;
        if (stat(fullpath, &st) == -1) continue;
        if (S_ISDIR(st.st_mode)) {
            scan_directory(fullpath);
        } else if (S_ISREG(st.st_mode)) {
            search_in_file(fullpath);
        }
    }
    closedir(dir);
}

void* thread_func(void* arg) {
    thread_arg_t* t = (thread_arg_t*)arg;
    scan_directory(t->path);
    return NULL;
}

void run_child_with_threads(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) exit(1);
    struct dirent* entry;
    char subdirs[MAX_THREADS][PATH_MAX];
    thread_arg_t args[MAX_THREADS];
    pthread_t tids[MAX_THREADS];
    int count = 0;

    while ((entry = readdir(dir)) && count < MAX_THREADS) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(subdirs[count], sizeof(subdirs[count]), "%s/%s", path, entry->d_name);
            strncpy(args[count].path, subdirs[count], PATH_MAX);
            pthread_create(&tids[count], NULL, thread_func, &args[count]);
            count++;
        }
    }
    closedir(dir);

    for (int i = 0; i < count; i++) {
        pthread_join(tids[i], NULL);
    }
}

int main(void) {
#if USE_MUTEX
    pthread_mutex_init(&print_lock, NULL);
#endif

    DIR* dir = opendir(ROOT_PATH);
    if (!dir) return 1;
    struct dirent* entry;
    char subdirs[MAX_CHILDREN][PATH_MAX];
    int count = 0;

    while ((entry = readdir(dir)) && count < MAX_CHILDREN) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(subdirs[count], sizeof(subdirs[count]), "%s/%s", ROOT_PATH, entry->d_name);
            count++;
        }
    }
    closedir(dir);

    for (int i = 0; i < count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            run_child_with_threads(subdirs[i]);
            exit(0);
        }
    }

    for (int i = 0; i < count; i++) {
        wait(NULL);
    }

#if USE_MUTEX
    pthread_mutex_destroy(&print_lock);
#endif

    return 0;
}