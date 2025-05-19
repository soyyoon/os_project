#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

    traverse_directory(root_dir, keyword, &result_list);

    sleep(2); // 스레드 종료 대기용 (간단한 대기 방식)

    printf("\n=== 검색 결과 ===\n");
    result_node_t* node = result_list.head;
    while (node) {
        printf("파일: %s (줄 %d): %s", node->result.filepath, node->result.line_number, node->result.line_content);
        node = node->next;
    }

    return 0;
}
