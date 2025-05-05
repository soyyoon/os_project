#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#define PATH_MAX 1024

// 디렉토리 재귀 순회 함수
void traverse_directory(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror("opendir 실패");
        return;
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        // 현재 디렉토리와 상위 디렉토리는 건너뜀
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // 전체 경로 구성
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        // 파일 정보 얻기
        struct stat statbuf;
        if (stat(full_path, &statbuf) == -1) {
            perror("stat 실패");
            continue;
        }

        // 디렉토리 재귀 호출
        if (S_ISDIR(statbuf.st_mode)) {
            printf("[디렉토리] %s\n", full_path);
            traverse_directory(full_path);
        }
        // 일반 파일은 출력만
        else if (S_ISREG(statbuf.st_mode)) {
            printf("[파일]      %s\n", full_path);
        }
    }


//디렉토리 탐색 함수 구현: 하위 디렉토리까지 재귀적으로 순회 기능 (파일 스레드 생성 X)스레드 생성, 동기화, 결과 리스트는 포함xxxx

    closedir(dir);
}
