#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define KEYWORD "apple"
#define DEFAULT_PATH "./dataset"
#define MAX_CHILDREN 8

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

int main(int argc, char* argv[]) {
    const char* path = (argc > 1) ? argv[1] : DEFAULT_PATH;

    DIR* dir = opendir(path);
    if (!dir) return 1;

    struct dirent* entry;
    char subdirs[MAX_CHILDREN][PATH_MAX];
    int count = 0;

    while ((entry = readdir(dir)) && count < MAX_CHILDREN) {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
            snprintf(subdirs[count++], sizeof(subdirs[count]), "%s/%s", path, entry->d_name);
        }
    }
    closedir(dir);

    for (int i = 1; i < count; i += 2) {
        pid_t pid = fork();
        if (pid == 0) {
            scan_directory(subdirs[i]);
            exit(0);
        }
    }

    // 부모는 짝수 인덱스 디렉토리 탐색
    for (int i = 0; i < count; i += 2) {
        scan_directory(subdirs[i]);
    }

    // 자식 종료 대기
    for (int i = 1; i < count; i += 2) {
        wait(NULL);
    }

    return 0;
}

