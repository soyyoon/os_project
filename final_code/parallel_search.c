// ✅ parallel_search.c — 부모도 참여, fork + thread + 동기화 구조

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define KEYWORD "apple"
#define DEFAULT_PATH "./dataset"
#define MAX_CHILDREN 4
#define MAX_THREADS 8

pthread_mutex_t print_lock;

typedef struct {
    char path[PATH_MAX];
} thread_arg_t;

void search_in_file(const char* filepath) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) return;

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, KEYWORD)) break;
    }
    fclose(fp);

    pthread_mutex_lock(&print_lock);
    printf("[SEARCH] %s\n", filepath);
    pthread_mutex_unlock(&print_lock);
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

void run_child_with_threads(char subdirs[][PATH_MAX], int start, int step, int total) {
    pthread_t tids[MAX_THREADS];
    thread_arg_t args[MAX_THREADS];
    int tcount = 0;

    for (int i = start; i < total && tcount < MAX_THREADS; i += step) {
        strncpy(args[tcount].path, subdirs[i], PATH_MAX);
        pthread_create(&tids[tcount], NULL, thread_func, &args[tcount]);
        tcount++;
    }

    for (int i = 0; i < tcount; i++) {
        pthread_join(tids[i], NULL);
    }
}

int main(int argc, char* argv[]) {
    pthread_mutex_init(&print_lock, NULL);

    const char* path = (argc > 1) ? argv[1] : DEFAULT_PATH;

    DIR* dir = opendir(path);
    if (!dir) return 1;

    char subdirs[MAX_CHILDREN][PATH_MAX];
    struct dirent* entry;
    int count = 0;

    while ((entry = readdir(dir)) && count < MAX_CHILDREN) {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            snprintf(subdirs[count], sizeof(subdirs[count]), "%s/%s", path, entry->d_name);
            count++;
        }
    }
    closedir(dir);

    pid_t pid = fork();
    if (pid == 0) {
        run_child_with_threads(subdirs, 1, 2, count); // child: 홀수
        exit(0);
    }

    run_child_with_threads(subdirs, 0, 2, count);     // parent: 짝수

    wait(NULL);
    pthread_mutex_destroy(&print_lock);
    return 0;
}

