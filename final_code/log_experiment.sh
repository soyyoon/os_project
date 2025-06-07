#!/bin/bash

log_dir="logs"
mkdir -p "$log_dir"
result_csv="${log_dir}/results.csv"

executables=(
  "./child_process"
  "./parent_only"
  "./single_thread"
  "./multi_process"
  "./multi_thread"
  "./parallel_search"
)

echo "실험명,실험조건,CPU활용률(%),real(sec),user(sec),sys(sec),mem(KB)" > "$result_csv"

# 실험 조건 정의 (단일 조건만 테스트할 경우엔 여기를 수정하면 돼요!)
declare -a depths=(12)
declare -a file_counts=(5000)
declare -a ratios=(1.0)
declare -a symlinks=("include")

for i in "${!depths[@]}"; do
  condition="D${depths[i]}_N${file_counts[i]}_M${ratios[i]}_S${symlinks[i]}"
  dataset_dir="dataset/${condition}"

  if [ ! -d "$dataset_dir" ]; then
    echo "⚠️ 폴더 없음: $dataset_dir, 스킵"
    continue
  fi

  for exe in "${executables[@]}"; do
    name=$(basename "$exe")
    tmp_log="${log_dir}/${name}_${condition}_tmp.log"
    perf_log="${log_dir}/${name}_${condition}_perf.log"

    echo "▶ 실행 중: $name / 조건: $condition"

    /usr/bin/time -v "$exe" "$dataset_dir" > "$tmp_log" 2> "$perf_log"

    real=$(grep "Elapsed" "$perf_log" | awk '{print $8}')
    user=$(grep "User time" "$perf_log" | awk '{print $4}')
    sys=$(grep "System time" "$perf_log" | awk '{print $4}')
    cpu=$(grep "Percent of CPU" "$perf_log" | awk '{print $8}' | tr -d '%')
    mem=$(grep "Maximum resident set size" "$perf_log" | awk '{print $6}')

    echo "${name},${condition},${cpu},${real},${user},${sys},${mem}" >> "$result_csv"
  done
done

echo "✅ 모든 테스트 완료! 결과: $result_csv"

