# GPUVision

GPUVision is a C++17 command-line simulator for GPU warp scheduling across Streaming Multiprocessors (SMs).

This repo currently includes Phase 1 plus Phase 2 command-line improvements. There is no frontend, JSON API, or visualization layer yet.

## Features

- Warp model with program counter, state, stall counter, and instruction stream
- `COMPUTE` and `MEMORY` instructions
- Configurable memory latency
- SM model where each SM can issue one instruction per cycle
- Round Robin, Oldest First, and Greedy Then Oldest schedulers
- Deterministic randomized workload generation
- Benchmark mode that compares all schedulers on the same workload
- CSV export for benchmark results
- Google Test coverage for schedulers, engine behavior, workload determinism, benchmark fairness, and CSV headers

## Metrics

- Total cycles
- Instructions completed
- IPC
- SM utilization percentage
- Idle cycles
- Memory stall count
- Completed warps
- Average warp completion time
- Min and max warp completion time
- Total SM cycles
- Non-idle SM cycles

SM utilization is calculated as:

```text
non_idle_sm_cycles / total_sm_cycles * 100
```

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run Demo Mode

Demo mode preserves the Phase 1 behavior: it creates 8 fixed warps, uses 2 SMs, prints cycle-by-cycle logs, and prints final metrics.

```sh
./build/gpuvision --demo
```

If no arguments are provided, GPUVision defaults to demo mode:

```sh
./build/gpuvision
```

## Run Benchmark Mode

Benchmark mode creates one deterministic workload and runs it through each scheduler for a fair comparison.

```sh
./build/gpuvision --benchmark \
  --warps 64 \
  --instructions 50 \
  --memory-probability 0.25 \
  --memory-latency 20 \
  --sms 4 \
  --seed 42
```

Example output:

```text
Benchmark Results
Workload:
  Warps: 64
  Instructions per warp: 50
  Memory probability: 0.25
  Memory latency: 20
  SMs: 4

Scheduler Comparison:
Round Robin
  Total cycles: ...
  Instructions completed: ...
  IPC: ...
  SM utilization: ...
  Idle cycles: ...
  Memory stall count: ...
  Completed warps: ...
  Average warp completion time: ...
```

## Export CSV

```sh
./build/gpuvision --benchmark --export results.csv
```

CSV columns:

```text
scheduler,total_cycles,instructions_completed,ipc,sm_utilization,idle_cycles,memory_stalls,completed_warps,average_warp_completion_time
```

## Why Compare Schedulers

GPU warp schedulers affect how often SMs find useful work while other warps wait on memory. Running each scheduler on the same workload isolates scheduler behavior from workload randomness, making IPC, utilization, idle cycles, and completion time meaningful comparisons.

## Test

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

Google Test is fetched by CMake during configuration.
