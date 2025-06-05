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

echo "실험명,실험조건,real(sec),user(sec),sys(sec),CPU활용률(%),mem(KB),파일열기성공여부,검색성공여부" > "$result_csv"

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

  for exe in "${executables_abc[@]}" "${executables_de[@]}"; do
    name=$(basename "$exe")
    tmp_log="${log_dir}/${name}_${condition}_tmp.log"
    perf_log="${log_dir}/${name}_${condition}_perf.log"

    echo "▶ $name 실행 중, 조건: $condition"

    # 실행 및 성능 측정
    /usr/bin/time -v "$exe" "$dataset_dir" > "$tmp_log" 2> "$perf_log"

    # 성능 정보 추출
    real=$(grep "Elapsed" "$perf_log" | awk '{print $8}')
    user=$(grep "User time" "$perf_log" | awk '{print $4}')
    sys=$(grep "System time" "$perf_log" | awk '{print $4}')
    cpu=$(grep "Percent of CPU" "$perf_log" | awk '{print $8}' | tr -d '%')
    mem=$(grep "Maximum resident set size" "$perf_log" | awk '{print $6}')

    # 파일 열기 및 검색 성공 여부 추출
    file_opened=$(grep -q "input.txt opened" "$tmp_log" && echo "Yes" || echo "No")
    found=$(grep -q "FOUND" "$tmp_log" && echo "Yes" || echo "No")

    echo "${name},${condition},${real},${user},${sys},${cpu},${mem},${file_opened},${found}" >> "$result_csv"
  done
done
