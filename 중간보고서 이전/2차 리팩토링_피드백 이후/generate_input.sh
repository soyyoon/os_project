
#!/bin/bash

# 인자: 깊이, 파일 수, 키워드 포함 비율, 심볼릭 포함 여부
DEPTH=$1
FILE_COUNT=$2
RATIO=$3
SYMLINK=$4
KEYWORD="apple"
ROOT_DIR="test_root"

rm -rf $ROOT_DIR
mkdir -p $ROOT_DIR

# 재귀적으로 디렉토리 생성
make_dirs() {
  local base=$1
  local depth=$2
  if [ "$depth" -le 0 ]; then return; fi
  mkdir -p "$base/dir$depth"
  make_dirs "$base/dir$depth" $((depth - 1))
}
make_dirs $ROOT_DIR $DEPTH

# 키워드 포함 여부 결정
INCLUDE_COUNT=$(echo "$FILE_COUNT * $RATIO" | bc | cut -d'.' -f1)

# 파일 생성 (100KB 텍스트 파일)
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

# 심볼릭 링크 포함 옵션
if [ "$SYMLINK" = "include" ]; then
  ln -s "$ROOT_DIR/dir1" "$ROOT_DIR/symlink_to_dir1"
fi

echo "✅ 테스트 환경 생성 완료: $FILE_COUNT개 파일, $DEPTH단계 디렉토리, 키워드 포함 $INCLUDE_COUNT개"
