#!/bin/bash
# prove.sh - build everything, run the correctness tests, and show that the
# count-leading-zeros instruction is really in the fast build.
# Works on Arm (aarch64) and x86-64.   Usage: ./prove.sh
# Note: adapted by an AI assistant from my own measurement script, for the
# AI-generated rewrite in nlz.h and bench.c.
cd "$(dirname "$0")" || exit 1
CFLAGS="-O2 -march=native"
case "$(uname -m)" in
  aarch64) INS=clz ;;
  x86_64)  INS=lzcnt ;;
  *) echo "Unknown architecture: $(uname -m)"; exit 1 ;;
esac
LOG=prove_$(uname -m).txt
{
  echo "== machine: $(uname -srm)"
  gcc --version | head -1
  echo
  echo "== build: gcc $CFLAGS"
  gcc $CFLAGS bench.c -o nlz_loop             || { echo "BUILD FAILED: nlz_loop"; exit 1; }
  gcc $CFLAGS -DUSE_CLZ bench.c -o nlz_clz    || { echo "BUILD FAILED: nlz_clz"; exit 1; }
  gcc $CFLAGS test.c -o nlz_test              || { echo "BUILD FAILED: nlz_test"; exit 1; }
  gcc $CFLAGS -S bench.c -o nlz_loop.s        || exit 1
  gcc $CFLAGS -DUSE_CLZ -S bench.c -o nlz_clz.s || exit 1
  echo "built: nlz_loop  nlz_clz  nlz_test  nlz_loop.s  nlz_clz.s"
  echo
  echo "== correctness: ./nlz_test"
  ./nlz_test || { echo "TESTS FAILED"; exit 1; }
  echo
  echo "== '$INS' lines in nlz_loop.s   (should be 0: the loop has no special instruction)"
  grep -c "$INS" nlz_loop.s
  echo
  echo "== '$INS' lines in nlz_clz.s   (the instruction the fast build uses)"
  grep -n "$INS" nlz_clz.s
  if [ "$INS" = lzcnt ]; then
    echo
    echo "== AVX2 register uses in nlz_loop.s   (above 0 = gcc vectorised the loop)"
    grep -c "ymm" nlz_loop.s
  fi
} 2>&1 | tee "$LOG"
echo
echo "Saved to $(pwd)/$LOG"
