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
#define USE_MUTEX 1
#define MAX_DIRS 64

pthread_mutex_t print_lock;
pthread_mutex_t count_lock;

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
    char (*paths)[PATH_MAX] = arg;
    for (int i = 1; i < MAX_DIRS; i += 2) {
        if (paths[i][0] != '\0') {
            scan_directory(paths[i]);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
#if USE_MUTEX
    pthread_mutex_init(&print_lock, NULL);
    pthread_mutex_init(&count_lock, NULL);
#endif

    const char* root_path = (argc > 1) ? argv[1] : DEFAULT_PATH;

    char subdirs[MAX_DIRS][PATH_MAX] = {0};
    int count = 0;

    DIR* dir = opendir(root_path);
    if (!dir) return 1;

    struct dirent* entry;
    while ((entry = readdir(dir)) && count < MAX_DIRS) {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") &&
            strcmp(entry->d_name, "..")) {
            snprintf(subdirs[count++], PATH_MAX, "%s/%s", root_path, entry->d_name);
        }
    }
    closedir(dir);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, subdirs);

    for (int i = 0; i < MAX_DIRS; i += 2) {
        if (subdirs[i][0] != '\0') {
            scan_directory(subdirs[i]);
        }
    }

    pthread_join(tid, NULL);

#if USE_MUTEX
    pthread_mutex_destroy(&print_lock);
    pthread_mutex_destroy(&count_lock);
#endif

    return 0;
}

