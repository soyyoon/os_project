/*테스트용 입력 생성기 (단일 프로세스/단일 스레드)

검색 키워드(keyword) 를 입력하고 → Parent → Child → Thread로 전달되며, 각 파일을 검색

현재 단계에서는 사용자로부터 키워드를 입력받고, 임의 디렉토리 목록을 순회하며 해당 키워드를 검색

이 코드는:
	•	사용자에게 검색 키워드를 입력받고
	•	미리 정의된 디렉토리 배열을 순회
	•	각 디렉토리 내의 모든 파일에서 해당 키워드를 검색

	실제 검색 알고리즘 대신, 테스트를 위해 단순히 경로만 출력 (실제 검색 함수와 연동 예정)*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

#define MAX_DIRS 5
#define MAX_KEYWORD_LEN 128

// 테스트용 디렉토리 경로 목록
const char* test_dirs[MAX_DIRS] = {
    "./testdir1",
    "./testdir2",
    "./sample_data",
    "./docs",
    "./src"
};

// 키워드 포함 여부만 확인 (모의)
void search_keyword_in_dir(const char* dir_path, const char* keyword) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror("디렉토리 열기 실패");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // 일반 파일만 처리
        if (entry->d_type == DT_REG) {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

            // 여기에서 실제 검색 로직이 들어갈 예정
            printf("[검색 대상 파일] %s 에서 \"%s\" 검색 예정\n", full_path, keyword);
        }
    }

    closedir(dir);
}

int main() {
    char keyword[MAX_KEYWORD_LEN];

    printf("🔍 검색할 키워드를 입력하세요: ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) {
        fprintf(stderr, "키워드 입력 오류\n");
        return 1;
    }

    // 개행 문자 제거
    keyword[strcspn(keyword, "\n")] = '\0';

    printf("\n[검색 시작] 키워드: \"%s\"\n", keyword);

    for (int i = 0; i < MAX_DIRS; i++) {
        printf("\n📁 디렉토리: %s\n", test_dirs[i]);
        search_keyword_in_dir(test_dirs[i], keyword);
    }

    printf("\n테스트 입력기 종료\n");
    return 0;
}

