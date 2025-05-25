#!/bin/bash

# 기준값
DEFAULT_DEPTH=3
DEFAULT_FILE_COUNT=500
DEFAULT_RATIO=0.5
DEFAULT_SYMLINK="include"

# 변수별 조합 / 수정 필요하면 하시길
depth_cases=(1 5)
file_cases=(100 1000)
ratio_cases=(0.0 1.0)
symlink_cases=("exclude")

# Baseline 고정값
baseline_condition="D${DEFAULT_DEPTH}_N${DEFAULT_FILE_COUNT}_M${DEFAULT_RATIO}_S${DEFAULT_SYMLINK}"
./generate_input.sh $DEFAULT_DEPTH $DEFAULT_FILE_COUNT $DEFAULT_RATIO $DEFAULT_SYMLINK
echo "Baseline 생성 완료: $baseline_condition"

# 깊이만 변경
for d in "${depth_cases[@]}"; do
  ./generate_input.sh $d $DEFAULT_FILE_COUNT $DEFAULT_RATIO $DEFAULT_SYMLINK
  echo "깊이 변경 실험 생성 완료: D${d}"
done

# 파일 수만 변경
for n in "${file_cases[@]}"; do
  ./generate_input.sh $DEFAULT_DEPTH $n $DEFAULT_RATIO $DEFAULT_SYMLINK
  echo "파일 수 변경 실험 생성 완료: N${n}"
done

# 부합 비율만 변경
for r in "${ratio_cases[@]}"; do
  ./generate_input.sh $DEFAULT_DEPTH $DEFAULT_FILE_COUNT $r $DEFAULT_SYMLINK
  echo "부합 비율 변경 실험 생성 완료: M${r}"
done

# 심볼릭 링크만 변경
for s in "${symlink_cases[@]}"; do
  ./generate_input.sh $DEFAULT_DEPTH $DEFAULT_FILE_COUNT $DEFAULT_RATIO $s
  echo "심볼릭 링크 제외 실험 생성 완료: S=${s}"
done

echo "모든 실험 입력 데이터 생성 완료"
