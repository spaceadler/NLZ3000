# Codemap: nlz3000

← [README](README.md)

> Generated from commit `4fcb432` by `scripts/codemap.py`. Regenerate it from the workspace root with `python3 scripts/codemap.py nlz3000`; don't edit it by hand.

## Files

Each box is a source file and its function count; an arrow means calls into that file.

```mermaid
flowchart LR
    n_test_c["test.c · 3"]
    n_bench_c["bench.c · 2"]
    n_nlz_h["nlz.h · 2"]
    n_bench_sh["bench.sh · 1"]
    n_test_c -->|2| n_nlz_h
```

## Hubs

The most connected functions: where a change reaches furthest.

| Function | Where | Called by | Calls |
|---|---|---|---|
| `main()` | `test.c:40` | 0 | 4 |
| `check()` | `test.c:13` | 1 | 0 |
| `expected_sum()` | `test.c:23` | 1 | 0 |
| `main()` | `bench.c:36` | 0 | 1 |
| `nlz_clz()` | `nlz.h:32` | 1 | 0 |
| `nlz_loop()` | `nlz.h:17` | 1 | 0 |
| `now()` | `bench.c:29` | 1 | 0 |

```mermaid
flowchart LR
    n_test_main["main()<br/>test.c"]
    n_test_check["check()"]
    n_test_expected_sum["expected_sum()"]
    n_bench_main["main()<br/>bench.c"]
    n_nlz_nlz_clz["nlz_clz()"]
    n_nlz_nlz_loop["nlz_loop()"]
    n_bench_now["now()"]
    n_test_main --> n_test_check
    n_test_main --> n_test_expected_sum
    n_test_main --> n_nlz_nlz_clz
    n_test_main --> n_nlz_nlz_loop
    n_bench_main --> n_bench_now
```

## Functions

### `bench.c`

- `now()` L29
- `main()` L36 → `now()`

### `bench.sh`

- `temps()` L12

### `nlz.h`

- `nlz_loop()` L17
- `nlz_clz()` L32

### `test.c`

- `check()` L13
- `expected_sum()` L23
- `main()` L40 → `check()`, `expected_sum()`, `nlz_clz()`, `nlz_loop()`
