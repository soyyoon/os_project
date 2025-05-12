// search_in_file.c - 파일 내 키워드 검색(기본용)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 검색 함수: 특정 파일에서 키워드의 등장 횟수와 줄 수를 계산
int search_in_file(const char* filepath, const char* keyword, int* match_count, int* line_count) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) {
        perror(filepath);  // 파일 열기 실패 시 에러 출력
        return -1;
    }

    char line[1024];
    *match_count = 0;
    *line_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, keyword)) {
            (*line_count)++;
            char* ptr = line;
            while ((ptr = strstr(ptr, keyword))) {
                (*match_count)++;
                ptr += strlen(keyword);
            }
        }
    }

    fclose(fp);
    return 0;
}

// 테스트용 main 함수
// int main(int argc, char* argv[]) {
//     if (argc != 3) {
//         fprintf(stderr, "사용법: %s <키워드> <파일경로>\n", argv[0]);
//         return 1;
//     }

//     const char* keyword = argv[1];
//     const char* filepath = argv[2];

//     int matches = 0;
//     int lines = 0;

//     if (search_in_file(filepath, keyword, &matches, &lines) == 0) {
//         printf("파일: %s\n", filepath);
//         printf(" - 키워드 \"%s\" 총 등장 횟수: %d\n", keyword, matches);
//         printf(" - 키워드 포함 줄 수: %d\n", lines);
//     } else {
//         fprintf(stderr, "검색 실패!\n");
//     }

//     return 0;
// }
