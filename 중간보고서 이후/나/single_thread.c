// [나] single_thread.c — 단일 thread 구조 (재귀 탐색 + 동기화 ON/OFF)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#define KEYWORD "target"
#define ROOT_PATH "./dataset"
#define USE_MUTEX 1 // 0으로 바꾸면 비동기 실행
#define MAX_DIRS 64

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
    printf("[Thread] %s: %s\n", filepath, found ? "FOUND" : "NOT FOUND");
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

// 단일 thread가 실행할 함수 - 홀수 인덱스 디렉토리 담당
void* thread_func(void* arg) {
    char (*paths)[PATH_MAX] = arg;
    for (int i = 1; i < MAX_DIRS; i += 2) {
        if (paths[i][0] != '\0') {
            scan_directory(paths[i]);
        }
    }
    return NULL;
}

int main(void) {
#if USE_MUTEX
    pthread_mutex_init(&print_lock, NULL);
#endif

    char subdirs[MAX_DIRS][PATH_MAX] = {0};
    int count = 0;

    DIR* dir = opendir(ROOT_PATH);
    if (!dir) return 1;

    struct dirent* entry;
    while ((entry = readdir(dir)) && count < MAX_DIRS) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            snprintf(subdirs[count++], PATH_MAX, "%s/%s", ROOT_PATH, entry->d_name);
        }
    }
    closedir(dir);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, subdirs);

    // main thread는 짝수 인덱스 디렉토리 탐색
    for (int i = 0; i < MAX_DIRS; i += 2) {
        if (subdirs[i][0] != '\0') {
            scan_directory(subdirs[i]);
        }
    }

    pthread_join(tid, NULL);

#if USE_MUTEX
    pthread_mutex_destroy(&print_lock);
#endif
    return 0;
}