#!/bin/bash

# 로그 저장 디렉토리
log_dir="logs"
mkdir -p "$log_dir"
result_csv="${log_dir}/results.csv"

# 측정 대상 실행파일 목록
executables=(
  "./child_once"
  "./single_process"
  "./single_thread"
  "./multi_process"
  "./multi_thread"
  "./parallel_search"
)

# CSV 헤더 작성
echo "실험명,실험조건,real(sec),user(sec),sys(sec),CPU활용률(%),mem(KB)" > "$result_csv"

# 통제 실험 조건 (수동 입력 or 자동화 가능)
condition="깊이2_1000개파일_부합50%_symlink없음"

for exe in "${executables[@]}"; do
  name=$(basename "$exe")
  tmp_log="${log_dir}/${name}_tmp.log"

  echo "▶ 실험 중: $name"

  # 실행 + 측정
  /usr/bin/time -f "%e,%U,%S,%M" -o "$tmp_log" "$exe" > /dev/null 2>&1

  if [[ -s "$tmp_log" ]]; then
    read real user sys mem < <(cat "$tmp_log" | tr ',' ' ')
    cpu_usage=$(awk "BEGIN {printf \"%.1f\", (($user + $sys) / $real) * 100}")
    echo "$name,$condition,$real,$user,$sys,$cpu_usage,$mem" >> "$result_csv"
  else
    echo "$name,$condition,실패,실패,실패,실패,실패" >> "$result_csv"
  fi
done

echo "✔ 모든 실험 완료: 결과는 $result_csv 에 저장됨"
