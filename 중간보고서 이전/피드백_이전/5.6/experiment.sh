#!/bin/bash

# 실행 파일 이름들
GEN_INPUT=./generate_input       # 입력 생성기
SINGLE=./single_thread_search    # 싱글스레드 실행 파일
MULTI=./multi_thread_search      # 멀티스레드 실행 파일

KEYWORD="test"                   # 검색 키워드
LOG_FILE="experiment_log.csv"

# 로그 헤더 초기화
echo "실행모드,스레드수,파일수,디렉토리깊이,부합비율,심볼릭포함,실행시간(초)" > $LOG_FILE

# 파라미터 루프
for T in 1 2 4 8 16; do                        # 스레드 수
for N in 100 1000 5000 10000; do              # 파일 수
for D in 1 3 5; do                             # 디렉토리 깊이
for M in 0 10 50 100; do                      # 파일명 조건 부합 비율
for S in include exclude; do                  # 심볼릭 링크 포함 여부

    echo "▶️ 실행: T=$T, N=$N, D=$D, M=$M%, 심볼릭=$S"

    # 입력 구조 생성 (generate_input 실행 시 인자 구조에 맞게 수정 필요)
    $GEN_INPUT $D $N $(echo "$M/100" | bc -l) $S

    # --- 싱글 스레드 실행 ---
    START=$(date +%s.%N)
    echo "$KEYWORD" | $SINGLE > /dev/null
    END=$(date +%s.%N)
    TIME_SINGLE=$(echo "$END - $START" | bc)
    echo "single,$T,$N,$D,$M,$S,$TIME_SINGLE" >> $LOG_FILE

    # --- 멀티 스레드 실행 ---
    START=$(date +%s.%N)
    echo "$KEYWORD" | $MULTI $T > /dev/null
    END=$(date +%s.%N)
    TIME_MULTI=$(echo "$END - $START" | bc)
    echo "multi,$T,$N,$D,$M,$S,$TIME_MULTI" >> $LOG_FILE

done; done; done; done; done

echo "✅ 실험 완료. 로그: $LOG_FILE"

#	generate_input 프로그램이 다음 인자 형식을 지원해야 함:
#./generate_input <directory_depth> <file_count> <match_ratio (0.0~1.0)> <include_symlinks>
#	•	single_thread_search와 multi_thread_search는 표준 입력으로 키워드를 받음
#multi_thread_search는 스레드 수를 인자로 받을 수 있어야 함
#→ 예: ./multi_thread_search 8
