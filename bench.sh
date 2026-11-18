#!/bin/bash
# bench.sh - time both builds: 4 runs on an idle machine, then 1 run while
# every hardware thread is kept busy by `yes > /dev/null`.
# Run ./prove.sh first (it builds the programs). Stop every other program and
# service you can, then:  ./bench.sh
cd "$(dirname "$0")" || exit 1
for p in ./nlz_loop ./nlz_clz; do
  [ -x "$p" ] || { echo "Missing $p: run ./prove.sh first"; exit 1; }
done
temps() {
  if command -v vcgencmd >/dev/null; then          # Raspberry Pi
    vcgencmd measure_temp; vcgencmd get_throttled
  elif command -v sensors >/dev/null; then         # x86 with lm_sensors
    sensors 2>/dev/null | grep -i -E 'tctl|package'
  fi
}
LOG=results_$(uname -m).txt
{
  echo "== machine: $(uname -srm)"; date
  gcc --version | head -1
  for prog in ./nlz_loop ./nlz_clz; do
    for i in 1 2 3 4; do
      echo "== $prog run $i (idle)"
      temps
      "$prog"
    done
    echo "== $prog run 5 (loaded: $(nproc) x yes)"
    for j in $(seq "$(nproc)"); do yes > /dev/null & done
    sleep 2
    temps
    "$prog"
    kill $(jobs -p) 2>/dev/null; wait 2>/dev/null
    sleep 5
  done
  echo "== done: $(date)"
} 2>&1 | tee "$LOG"

# Means and speedups, from the log just written.
awk '
  /^== \.\/nlz_/ { prog = $2 }
  /^Time:/       { t[prog] = t[prog] " " $2 }
  END {
    for (p in t) {
      n = split(t[p], v, " "); s = 0; l = 0
      for (i = 1; i <= n; i++) { s += v[i]; l += log(v[i]) }
      am[p] = s / n; gm[p] = exp(l / n)
      printf "%-11s arithmetic mean %.4f s   geometric mean %.4f s\n", p, am[p], gm[p]
    }
    if (("./nlz_loop" in am) && ("./nlz_clz" in am))
      printf "speedup     arithmetic %.2fx   geometric %.2fx\n",
             am["./nlz_loop"] / am["./nlz_clz"], gm["./nlz_loop"] / gm["./nlz_clz"]
  }' "$LOG" | tee -a "$LOG"
echo
echo "Saved to $(pwd)/$LOG"
