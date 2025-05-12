
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH 4096
pthread_mutex_t print_lock;

void search_dir_safe(const char *path, const char *keyword) {
DIR *dir;
struct dirent *entry;
struct stat statbuf;
char fullpath[MAX_PATH];
dir = opendir(path);
if (!dir) {
    pthread_mutex_lock(&print_lock);
    fprintf(stderr, "디렉토리 열기 실패: %s (%s)\\n", path, strerror(errno));
    pthread_mutex_unlock(&print_lock);
    return;
}

while ((entry = readdir(dir)) != NULL) {
    // 현재/부모 디렉토리 무시
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

    // 심볼릭 링크 무시 (원하면 따로 처리 가능)
    if (lstat(fullpath, &statbuf) == -1) {
        pthread_mutex_lock(&print_lock);
        fprintf(stderr, "lstat 실패: %s (%s)\\n", fullpath, strerror(errno));
        pthread_mutex_unlock(&print_lock);
        continue;
    }

    if (S_ISLNK(statbuf.st_mode)) {
        pthread_mutex_lock(&print_lock);
        fprintf(stderr, "🔗 링크 무시: %s\\n", fullpath);
        pthread_mutex_unlock(&print_lock);
        continue;
    }

    if (S_ISDIR(statbuf.st_mode)) {
        // 재귀 호출
        search_dir_safe(fullpath, keyword);
    } else if (S_ISREG(statbuf.st_mode)) {
        // 파일명에 키워드가 포함되면 출력
        if (strstr(entry->d_name, keyword)) {
            pthread_mutex_lock(&print_lock);
            printf("발견: %s\\n", fullpath);
            pthread_mutex_unlock(&print_lock);
        }
    }
}

closedir(dir);
}
