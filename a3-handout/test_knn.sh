#!/bin/bash

check_command() {
  if [ $? -ne 0 ]; then
    echo "Error executing $1"
    exit 1
  fi
}
# P Q D K
declare -a COMBINATIONS=(
    # Edges
    "0 0 1 1"
    "1 0 1 1"
    "0 1 1 1"
    "1 1 1 1"

    # Small
    "5 1 2 3"
    "10 5 1 3"
    "10 5 2 3"
    "10 5 3 3"

    # Medium
    "100 50 1 30"
    "100 50 2 30"
    "100 50 3 30"

    # Large
    "1000 500 1 300"
    "1000 500 2 300"

    # Role the dice
    "40 30 20 10"
    "10 20 30 40"

)

echo "Running small, medium and large tests."
echo "P=Points, Q=Queries, D=Dimensions, K=K value"
echo "Smaller than 10 point and 10 query gets a SVG plot"
echo ""

identical_count=0

bruteforce_faster_count=0
kdtree_faster_count=0

for combo in "${COMBINATIONS[@]}"
do
  read NUM_POINTS NUM_QUERIES DIM K <<< $combo

  # Generate
  ./knn-genpoints $NUM_POINTS $DIM > points
  check_command "knn-genpoints for points"
  ./knn-genpoints $NUM_QUERIES $DIM > queries
  check_command "knn-genpoints for queries"

  # Time
  bf_time=$( { time ./knn-bruteforce points queries $K indexes_bruteforce; } 2>&1 )
  check_command "knn-bruteforce"
  kd_time=$( { time ./knn-kdtree points queries $K indexes_kdtree; } 2>&1 )
  check_command "knn-kdtree"
  bf_real_time=$(echo "$bf_time" | grep real | awk '{print $2}')
  kd_real_time=$(echo "$kd_time" | grep real | awk '{print $2}')

  # Compare output
  if cmp -s indexes_bruteforce indexes_kdtree; then
    identical_count=$((identical_count + 1))
    result="identical"
  else
    result="different"
  fi


  # SVG
  if [ $NUM_POINTS -le 10 ] && [ $NUM_QUERIES -le 10 ] && [ $DIM -eq 2 ]; then
    ./knn-svg points queries indexes_kdtree > ${NUM_POINTS}P_${NUM_QUERIES}Q_${DIM}D_${K}K.svg
  fi

  convert_to_milliseconds() {
    local time=$1
    local minutes=$(echo $time | cut -d'm' -f1)
    local seconds=$(echo $time | cut -d'm' -f2 | sed 's/s//' | awk '{printf "%d\n", $1 * 1000}')
    echo $((minutes * 60000 + seconds))
}

# Compare times
bf_milliseconds=$(convert_to_milliseconds $bf_real_time)
kd_milliseconds=$(convert_to_milliseconds $kd_real_time)
if [ $bf_milliseconds -lt $kd_milliseconds ]; then
  bruteforce_faster_count=$((bruteforce_faster_count + 1))
else
  kdtree_faster_count=$((kdtree_faster_count + 1))
fi

# print
  echo "P=$NUM_POINTS, Q=$NUM_QUERIES, D=$DIM, K=$K | result = $result."
  echo "Bruteforce time: $bf_real_time | kdtree time: $kd_real_time"


done
echo ""
echo "SUMMARY"
echo "Total tests: ${#COMBINATIONS[@]}"
echo "Identical outputs: $identical_count"
echo "Bruteforce faster: $bruteforce_faster_count | Kdtree faster: $kdtree_faster_count"

