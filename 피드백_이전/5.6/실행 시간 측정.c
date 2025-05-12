#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>           // ✅ 실행 시간 측정용
#include "file_search.h"

int main() {
    char keyword[256];
    printf("🔍 검색할 키워드를 입력하세요: ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) {
        fprintf(stderr, "키워드 입력 실패\n");
        return 1;
    }
    keyword[strcspn(keyword, "\n")] = '\0';

    result_list_t result_list = {0};
    const char* root_dir = "./test_root";

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);   // ✅ 시작 시간 기록

    traverse_directory(root_dir, keyword, &result_list);

    sleep(2); // 스레드 종료 대기용 (간단한 대기 방식)

    gettimeofday(&end_time, NULL);     // ✅ 종료 시간 기록

    printf("\n=== 검색 결과 ===\n");
    result_node_t* node = result_list.head;
    while (node) {
        printf("파일: %s (줄 %d): %s", node->result.filepath, node->result.line_number, node->result.line_content);
        node = node->next;
    }

    // ✅ 시간 차 계산 및 출력
    double elapsed_sec = (end_time.tv_sec - start_time.tv_sec) +
                         (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("\n⏱️ 실행 시간: %.6f초\n", elapsed_sec);

    return 0;
}
