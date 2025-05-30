#!/bin/bash

log_dir="logs"
mkdir -p "$log_dir"
result_csv="${log_dir}/results.csv"

executables_abc=(  # [가],[나] 대상
  "./child_process"
  "./parent_only"
  "./single_thread"
)

executables_de=(  # [다],[라] 대상
  "./multi_process"
  "./multi_thread"
  "./parallel_search"
)

echo "실험명,실험조건,real(sec),user(sec),sys(sec),CPU활용률(%),mem(KB)" > "$result_csv"

# [가],[나] 5가지 변수 조합 폴더
declare -a depths=(3 5 3 3 3)
declare -a file_counts=(500 500 1000 500 500)
declare -a ratios=(0.5 0.5 0.5 1.0 0.5)
declare -a symlinks=("include" "include" "include" "include" "exclude")

for i in ${!depths[@]}; do
  condition="D${depths[i]}_N${file_counts[i]}_M${ratios[i]}_S${symlinks[i]}"
  dataset_dir="dataset/${condition}"

  if [ ! -d "$dataset_dir" ]; then
    echo "폴더 없음: $dataset_dir, 스킵"
    continue
  fi

  for exe in "${executables_abc[@]}"; do
    name=$(basename "$exe")
    tmp_log="${log_dir}/${name}_${condition}_tmp.log"
    echo "▶ $name 실행 중, 조건: $condition"

    # macOS time output parsing (stderr)
    { /usr/bin/time -p "$exe" "$dataset_dir"; } 2> "$tmp_log" 1> /dev/null

    if grep -q "^real" "$tmp_log"; then
      real=$(grep real "$tmp_log" | awk '{print $2}')
      user=$(grep user "$tmp_log" | awk '{print $2}')
      sys=$(grep sys "$tmp_log" | awk '{print $2}')
      cpu_usage=$(awk "BEGIN {printf \"%.1f\", (($user + $sys) / $real) * 100}")
      mem="N/A"  # macOS 기본 time은 메모리 정보 없음
      echo "$name,$condition,$real,$user,$sys,$cpu_usage,$mem" >> "$result_csv"
    else
      echo "$name,$condition,실패,실패,실패,실패,실패" >> "$result_csv"
    fi
  done
done

# [다],[라]는 고정 조건에서만 실행
fixed_condition="D3_N500_M0.5_Sinclude"
fixed_dir="dataset/${fixed_condition}"

if [ -d "$fixed_dir" ]; then
  for exe in "${executables_de[@]}"; do
    name=$(basename "$exe")
    tmp_log="${log_dir}/${name}_${fixed_condition}_tmp.log"
    echo "▶ $name 실행 중, 조건: $fixed_condition"

    { /usr/bin/time -p "$exe" "$fixed_dir"; } 2> "$tmp_log" 1> /dev/null

    if grep -q "^real" "$tmp_log"; then
      real=$(grep real "$tmp_log" | awk '{print $2}')
      user=$(grep user "$tmp_log" | awk '{print $2}')
      sys=$(grep sys "$tmp_log" | awk '{print $2}')
      cpu_usage=$(awk "BEGIN {printf \"%.1f\", (($user + $sys) / $real) * 100}")
      mem="N/A"
      echo "$name,$fixed_condition,$real,$user,$sys,$cpu_usage,$mem" >> "$result_csv"
    else
      echo "$name,$fixed_condition,실패,실패,실패,실패,실패" >> "$result_csv"
    fi
  done
else
  echo "고정 조건 폴더 없음: $fixed_dir"
fi

echo "✔ 모든 실험 완료: 결과는 $result_csv 에 저장됨"