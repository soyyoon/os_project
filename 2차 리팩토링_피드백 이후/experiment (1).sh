#!/bin/bash

KEYWORD="apple"
LOG_FILE="experiment_log.csv"
CPU_LOG_DIR="cpu_logs"

SINGLE=./file_search_single
MULTI=./file_search
GEN_INPUT=./generate_input.sh

mkdir -p $CPU_LOG_DIR
echo "실행모드,파일수,디렉토리깊이,부합비율,심볼릭포함,실행시간(초),UserTime,SysTime,CPU(%),MaxMemory(KB)" > $LOG_FILE

for N in 100 500; do
for D in 1 3; do
for M in 0 50 100; do
for S in include exclude; do

    echo "▶️ 실행: N=$N, D=$D, M=$M%, 심볼릭=$S"
    $GEN_INPUT $D $N $(echo "$M/100" | bc -l) $S

    # 싱글 스레드
    /usr/bin/time -v $SINGLE $KEYWORD --ext=.txt --max-depth=$D       1> /dev/null       2> $CPU_LOG_DIR/single.log

    TIME_SINGLE=$(grep "Elapsed (wall clock) time" $CPU_LOG_DIR/single.log | awk '{print $8}')
    USER_TIME=$(grep "User time (seconds)" $CPU_LOG_DIR/single.log | awk '{print $5}')
    SYS_TIME=$(grep "System time (seconds)" $CPU_LOG_DIR/single.log | awk '{print $5}')
    CPU_PCT=$(grep "Percent of CPU this job got" $CPU_LOG_DIR/single.log | awk '{print $8}' | tr -d '%')
    MAX_MEM=$(grep "Maximum resident set size" $CPU_LOG_DIR/single.log | awk '{print $6}')

    echo "single,$N,$D,$M,$S,$TIME_SINGLE,$USER_TIME,$SYS_TIME,$CPU_PCT,$MAX_MEM" >> $LOG_FILE

    # 멀티 스레드
    /usr/bin/time -v $MULTI $KEYWORD --ext=.txt --max-depth=$D       1> /dev/null       2> $CPU_LOG_DIR/multi.log

    TIME_MULTI=$(grep "Elapsed (wall clock) time" $CPU_LOG_DIR/multi.log | awk '{print $8}')
    USER_TIME=$(grep "User time (seconds)" $CPU_LOG_DIR/multi.log | awk '{print $5}')
    SYS_TIME=$(grep "System time (seconds)" $CPU_LOG_DIR/multi.log | awk '{print $5}')
    CPU_PCT=$(grep "Percent of CPU this job got" $CPU_LOG_DIR/multi.log | awk '{print $8}' | tr -d '%')
    MAX_MEM=$(grep "Maximum resident set size" $CPU_LOG_DIR/multi.log | awk '{print $6}')

    echo "multi,$N,$D,$M,$S,$TIME_MULTI,$USER_TIME,$SYS_TIME,$CPU_PCT,$MAX_MEM" >> $LOG_FILE

done; done; done; done

echo "✅ 실험 완료. 결과는 $LOG_FILE 에 저장됨."
