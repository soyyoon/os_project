#!/bin/bash

# 기본 고정 조건
BASE_DEPTH=3
BASE_FILE_COUNT=500
BASE_RATIO=0.5
BASE_SYMLINK="include"

# 5가지 환경 배열 (하나씩 변수 변경)
declare -a depths=($BASE_DEPTH 5 $BASE_DEPTH $BASE_DEPTH $BASE_DEPTH)
declare -a file_counts=($BASE_FILE_COUNT $BASE_FILE_COUNT 1000 $BASE_FILE_COUNT $BASE_FILE_COUNT)
declare -a ratios=($BASE_RATIO $BASE_RATIO $BASE_RATIO 1.0 $BASE_RATIO)
declare -a symlinks=($BASE_SYMLINK $BASE_SYMLINK $BASE_SYMLINK $BASE_SYMLINK "exclude")

for i in ${!depths[@]}; do
  ./generate_input.sh "${depths[i]}" "${file_counts[i]}" "${ratios[i]}" "${symlinks[i]}"
done

echo "✅ 5가지 테스트 환경 데이터 생성 완료"
