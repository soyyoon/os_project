#!/bin/bash

# 실행 파일 이름들 (현재 구조에 맞게 수정)
GEN_INPUT=./generate_input           # 입력 생성기
SINGLE=./file_search_single          # 싱글 스레드 버전
MULTI=./file_search                  # 멀티 스레드 버전 (스레드 수 고정)

KEYWORD="test"                       # 검색 키워드
LOG_FILE="experiment_log.csv"

# 로그 헤더 초기화
echo "실행모드,스레드수,파일수,디렉토리깊이,부합비율,심볼릭포함,실행시간(초)" > $LOG_FILE

# 파라미터 루프
for T in 1 2 4 8; do                        # 실험 목적용 스레드 수 표시용 (실제로는 코드 내 고정)
for N in 100 500 1000; do                  # 파일 수
for D in 1 3; do                            # 디렉토리 깊이
for M in 0 50 100; do                      # 키워드 포함 비율 (%)
for S in include exclude; do              # 심볼릭 링크 포함 여부

    echo "▶️ 실행: T=$T, N=$N, D=$D, M=$M%, 심볼릭=$S"

    # 입력 구조 생성
    $GEN_INPUT $D $N $(echo "$M/100" | bc -l) $S

    # 싱글 스레드 실행
    START=$(date +%s.%N)
    echo "$KEYWORD" | $SINGLE --ext=.txt --max-depth=$D > /dev/null
    END=$(date +%s.%N)
    TIME_SINGLE=$(echo "$END - $START" | bc)
    echo "single,$T,$N,$D,$M,$S,$TIME_SINGLE" >> $LOG_FILE

    # 멀티 스레드 실행
    START=$(date +%s.%N)
    echo "$KEYWORD" | $MULTI --ext=.txt --max-depth=$D > /dev/null
    END=$(date +%s.%N)
    TIME_MULTI=$(echo "$END - $START" | bc)
    echo "multi,$T,$N,$D,$M,$S,$TIME_MULTI" >> $LOG_FILE

done; done; done; done; done

echo "✅ 실험 완료. 결과는 $LOG_FILE 에 저장됨."
