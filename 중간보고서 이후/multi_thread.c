// ✅ [다] multi_thread.c — 여러 thread로 분할 (main thread도 디렉토리 탐색 참여)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#define KEYWORD "apple"
#define DEFAULT_PATH "./dataset"
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
    printf("[Thread %d] %s: %s", getpid(), filepath, found ? "FOUND" : "NOT FOUND");
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

int main(int argc, char* argv[]) {
#if USE_MUTEX
    pthread_mutex_init(&print_lock, NULL);
#endif

    const char* path = (argc > 1) ? argv[1] : DEFAULT_PATH;

    DIR* dir = opendir(path);
    if (!dir) return 1;
    struct dirent* entry;
    char subdirs[MAX_THREADS][PATH_MAX];
    thread_arg_t args[MAX_THREADS];
    pthread_t tids[MAX_THREADS];
    int count = 0;

    while ((entry = readdir(dir)) && count < MAX_THREADS) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(subdirs[count], sizeof(subdirs[count]), "%s/%s", path, entry->d_name);
            strncpy(args[count].path, subdirs[count], PATH_MAX);
            count++;
        }
    }
    closedir(dir);

    for (int i = 1; i < count; i += 2) {
        pthread_create(&tids[i], NULL, thread_func, &args[i]);
    }

    for (int i = 0; i < count; i += 2) {
        scan_directory(subdirs[i]);
    }

    for (int i = 1; i < count; i += 2) {
        pthread_join(tids[i], NULL);
    }

#if USE_MUTEX
    pthread_mutex_destroy(&print_lock);
#endif
    return 0;
}
