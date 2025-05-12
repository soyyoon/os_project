#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define ROOT_DIR "test_root"
#define DIR_COUNT 20
#define FILES_PER_DIR 50
#define MATCH_RATE 0.1

void make_dir(const char *path, mode_t mode) {
    if (mkdir(path, mode) == -1 && errno != EEXIST) {
        perror(path);
    }
}

void make_file(const char *path) {
    int fd = open(path, O_CREAT | O_WRONLY, 0644);
    if (fd >= 0) {
        write(fd, "test content\n", 13);
        close(fd);
    } else {
        perror(path);
    }
}

int main() {
    srand(time(NULL));
    char path[512];

    // 기존 디렉토리 제거
    system("rm -rf " ROOT_DIR);
    make_dir(ROOT_DIR, 0755);

    for (int i = 1; i <= DIR_COUNT; i++) {
        snprintf(path, sizeof(path), ROOT_DIR"/dir_%02d", i);
        make_dir(path, 0755);

        for (int j = 1; j <= FILES_PER_DIR; j++) {
            char file_path[512];
            double r = (double)rand() / RAND_MAX;
            if (r < MATCH_RATE) {
                snprintf(file_path, sizeof(file_path), "%s/match_%03d.txt", path, j);
            } else {
                snprintf(file_path, sizeof(file_path), "%s/file_%03d.log", path, j);
            }
            make_file(file_path);
        }

        if (i % 2 == 0) {
            snprintf(path, sizeof(path), ROOT_DIR"/dir_%02d/sub", i);
            make_dir(path, 0755);
            char file_path[512];
            snprintf(file_path, sizeof(file_path), "%s/match_nested_%02d.txt", path, i);
            make_file(file_path);
        }
    }

    // 빈 폴더
    make_dir(ROOT_DIR"/empty_folder", 0755);

    // 한글/특수문자 폴더 및 파일
    make_dir(ROOT_DIR"/한글폴더", 0755);
    make_file(ROOT_DIR"/한글폴더/특수파일.txt");

    // 권한 제한 폴더
    make_dir(ROOT_DIR"/restricted", 0000);

    // 심볼릭 링크 폴더
    make_dir(ROOT_DIR"/real_folder", 0755);
    make_file(ROOT_DIR"/real_folder/match_symlink.txt");
    symlink("real_folder", ROOT_DIR"/link_folder");

    printf("입력 구조 생성 완료\n");
    return 0;
}
