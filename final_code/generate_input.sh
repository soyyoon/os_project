#!/bin/bash

# 매개변수 확인
if [ "$#" -ne 4 ]; then
  echo "❌ 사용법: $0 <DEPTH> <FILE_COUNT> <RATIO> <SYMLINK(include|exclude)>"
  exit 1
fi

DEPTH=$1  
FILE_COUNT=$2
RATIO=$3
SYMLINK=$4
KEYWORD="apple"
ROOT_DIR="dataset/D${DEPTH}_N${FILE_COUNT}_M${RATIO}_S${SYMLINK}"

rm -rf "$ROOT_DIR"
mkdir -p "$ROOT_DIR"

# ❶ 디렉토리 생성 (넓게 퍼지게)
make_dirs() {
  local base=$1
  local depth=$2
  if [ "$depth" -le 0 ]; then return; fi
  for i in 1 2 3; do
    local newdir="${base}/dir${depth}_$i"
    mkdir -p "$newdir"
    make_dirs "$newdir" $((depth - 1))
  done
}
make_dirs "$ROOT_DIR" "$DEPTH"

# ❷ 전체 디렉토리 목록 수집 (macOS용)
ALL_DIRS=()
while IFS= read -r line; do
  ALL_DIRS+=("$line")
done < <(find "$ROOT_DIR" -type d)

# ❸ 포함 파일 수 계산
INCLUDE_COUNT=$(echo "$FILE_COUNT * $RATIO" | bc | cut -d'.' -f1)

# ❹ 파일 생성
for ((i=0; i<FILE_COUNT; i++)); do
  RANDOM_INDEX=$((RANDOM % ${#ALL_DIRS[@]}))
  TARGET_DIR="${ALL_DIRS[$RANDOM_INDEX]}"
  FILENAME="$TARGET_DIR/file_$i.txt"

  if [ "$i" -lt "$INCLUDE_COUNT" ]; then
    echo "this file contains $KEYWORD and some random words" | head -c 102400 > "$FILENAME"
  else
    echo "this is a random text without the keyword" | head -c 102400 > "$FILENAME"
  fi
done

# ❺ 심볼릭 링크 생성
if [ "$SYMLINK" = "include" ]; then
  ln -s "${ALL_DIRS[0]}" "$ROOT_DIR/symlink_to_first"
fi

echo "✅ 테스트 환경 생성 완료: $FILE_COUNT개 파일, $DEPTH단계 디렉토리, 키워드 포함 $INCLUDE_COUNT개, symlink=$SYMLINK"
