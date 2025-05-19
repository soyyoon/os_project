// 🔍 file_search_single.c - 싱글 스레드 버전 파일 검색기
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#define PATH_MAX 1024
#define MAX_LINE_LENGTH 1024

int count_words(const char* line) {
    int count = 0, in_word = 0;
    for (int i = 0; line[i]; i++) {
        if (isspace(line[i])) in_word = 0;
        else if (!in_word) { in_word = 1; count++; }
    }
    return count;
}

void search_in_file(const char* filepath, const char* keyword) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) return;

    char line[MAX_LINE_LENGTH];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        if (strstr(line, keyword)) {
            int word_count = count_words(line);
            printf("파일: %s (줄 %d, %d단어) → %s",
                filepath, line_num, word_count, line);
        }
    }

    fclose(fp);
}

void traverse_directory(const char* path, const char* keyword, const char* ext_filter, int depth, int max_depth) {
    if (depth > max_depth) return;

    DIR* dir = opendir(path);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (lstat(fullpath, &statbuf) == -1) continue;

        if (S_ISDIR(statbuf.st_mode)) {
            traverse_directory(fullpath, keyword, ext_filter, depth + 1, max_depth);
        } else if (S_ISREG(statbuf.st_mode)) {
            if (!ext_filter || strstr(entry->d_name, ext_filter)) {
                search_in_file(fullpath, keyword);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "사용법: %s <keyword> [--ext=.txt] [--max-depth=N]\n", argv[0]);
        return 1;
    }

    const char* keyword = argv[1];
    const char* root_dir = "./test_root";
    const char* ext_filter = NULL;
    int max_depth = 10;

    for (int i = 2; i < argc; i++) {
        if (strncmp(argv[i], "--ext=", 6) == 0) {
            ext_filter = argv[i] + 6;
        } else if (strncmp(argv[i], "--max-depth=", 12) == 0) {
            max_depth = atoi(argv[i] + 12);
        }
    }

    traverse_directory(root_dir, keyword, ext_filter, 0, max_depth);

    return 0;
}
