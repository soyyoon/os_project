#!/bin/bash

DEPTH=$1
FILE_COUNT=$2
RATIO=$3
SYMLINK=$4
KEYWORD="apple"
ROOT_DIR="dataset/D${DEPTH}_N${FILE_COUNT}_M${RATIO}_S${SYMLINK}"

rm -rf "$ROOT_DIR"
mkdir -p "$ROOT_DIR"

make_dirs() {
  local base=$1
  local depth=$2
  if [ "$depth" -le 0 ]; then return; fi
  mkdir -p "$base/dir$depth"
  make_dirs "$base/dir$depth" $((depth - 1))
}
make_dirs "$ROOT_DIR" "$DEPTH"

INCLUDE_COUNT=$(echo "scale=2; $FILE_COUNT * $RATIO" | bc | cut -d'.' -f1)

i=0
while [ $i -lt $FILE_COUNT ]; do
  PATH_DEPTH=$(( (RANDOM % DEPTH) + 1 ))
  TARGET_DIR="$ROOT_DIR"
  for ((d=1; d<=PATH_DEPTH; d++)); do
    TARGET_DIR="$TARGET_DIR/dir$d"
  done
  mkdir -p "$TARGET_DIR"

  FILENAME="$TARGET_DIR/file_$i.txt"
  if [ $i -lt $INCLUDE_COUNT ]; then
    yes "this file contains $KEYWORD and some random words" | head -c 102400 > "$FILENAME"
  else
    yes "this is a random text without the keyword" | head -c 102400 > "$FILENAME"
  fi
  i=$((i + 1))
done

if [ "$SYMLINK" = "include" ]; then
  ln -s "$ROOT_DIR/dir1" "$ROOT_DIR/symlink_to_dir1"
fi

echo "✅ 테스트 환경 생성 완료: $FILE_COUNT개 파일, $DEPTH단계 디렉토리, 키워드 포함 $INCLUDE_COUNT개, symlink=$SYMLINK"
