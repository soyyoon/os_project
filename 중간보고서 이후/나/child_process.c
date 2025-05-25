// [가] child_once.c — 단일 child + parent 병렬 탐색 (재귀 탐색)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define KEYWORD "target"
#define ROOT_PATH "./dataset"
#define MAX_DIRS 64

// 파일 하나를 열고, KEYWORD가 포함돼 있는지 줄 단위로 검사하는 함수
void search_in_file(const char* filepath, const char* role) {
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
    printf("[%s %d] %s: %s", role, getpid(), filepath, found ? "FOUND" : "NOT FOUND");
}

// 재귀 탐색 함수
void scan_directory(const char* path, const char* role) {
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
            scan_directory(fullpath, role);
        } else if (S_ISREG(st.st_mode)) {
            search_in_file(fullpath, role);
        }
    }
    closedir(dir);
}

int main(void) {
    // 하위 디렉토리 경로들을 저장할 배열
    char subdirs[MAX_DIRS][PATH_MAX];
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

    // 자식프로세스 생성
    pid_t pid = fork();
    if (pid == 0) {
        for (int i = 1; i < count; i += 2) // 수집된 디렉토리 중 홀수 인덱스 디렉토리 처리
            scan_directory(subdirs[i], "Child");
        exit(0);
    }

    for (int i = 0; i < count; i += 2) {// 수집된 디렉토리 중 짝수 인덱스 디렉토리 처리
        scan_directory(subdirs[i], "Parent");

    wait(NULL);
    return 0;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        scan_directory(ROOT_PATH, "Child");
        exit(0);
    }
    scan_directory(ROOT_PATH, "Parent");
    wait(NULL);
    return 0;
}
