# GPUVision

GPUVision is a C++17 command-line simulator for GPU warp scheduling across Streaming Multiprocessors (SMs).

This repo currently includes Phase 1, Phase 2 command-line improvements, and Phase 3 JSON output for API-ready simulation results. There is no frontend or web API server yet.

## Features

- Warp model with program counter, state, stall counter, and instruction stream
- `COMPUTE` and `MEMORY` instructions
- Configurable memory latency
- SM model where each SM can issue one instruction per cycle
- Round Robin, Oldest First, and Greedy Then Oldest schedulers
- Deterministic randomized workload generation
- Benchmark mode that compares all schedulers on the same workload
- CSV export for benchmark results
- Structured JSON output for demo and benchmark runs
- Optional per-cycle timeline capture for dashboard-ready demo output
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

## Run Demo JSON Mode

Demo JSON mode suppresses human-readable logs and emits one structured JSON document.

```sh
./build/gpuvision --demo --json
```

The demo JSON includes configuration, scheduler name, final metrics, per-cycle SM events, memory stall countdowns, completed warps, and warp state snapshots.

Example shape:

```json
{
  "mode": "demo",
  "scheduler": "Round Robin",
  "config": {
    "sms": 2,
    "warps": 8,
    "instructions_per_warp": 4,
    "memory_latency": 4
  },
  "final_metrics": {
    "total_cycles": 17,
    "instructions_completed": 28,
    "ipc": 1.647059,
    "sm_utilization": 82.352941
  },
  "timeline": [
    {
      "cycle": 0,
      "sms": [
        {
          "sm_id": 0,
          "warp_id": 0,
          "instruction": "COMPUTE",
          "resulting_state": "RUNNING",
          "idle": false
        }
      ]
    }
  ]
}
```

## Run Benchmark JSON Mode

Benchmark JSON mode runs the same deterministic workload through every scheduler and emits scheduler comparison data.

```sh
./build/gpuvision --benchmark --json \
  --warps 64 \
  --instructions 50 \
  --memory-probability 0.25 \
  --memory-latency 20 \
  --sms 4 \
  --seed 42
```

Example shape:

```json
{
  "mode": "benchmark",
  "workload": {
    "warps": 64,
    "instructions_per_warp": 50,
    "memory_probability": 0.25,
    "memory_latency": 20,
    "sms": 4,
    "seed": 42
  },
  "results": [
    {
      "scheduler": "Round Robin",
      "metrics": {
        "total_cycles": 847,
        "instructions_completed": 3200,
        "ipc": 3.778040,
        "sm_utilization": 94.451004
      }
    }
  ]
}
```

## Export JSON

When `--json` is present, `--export` writes JSON instead of CSV.

```sh
./build/gpuvision --demo --json --export demo.json
./build/gpuvision --benchmark --json --export results.json
```

Without `--json`, benchmark export remains CSV:

```sh
./build/gpuvision --benchmark --export results.csv
```

## Why Compare Schedulers

GPU warp schedulers affect how often SMs find useful work while other warps wait on memory. Running each scheduler on the same workload isolates scheduler behavior from workload randomness, making IPC, utilization, idle cycles, and completion time meaningful comparisons.

## Dashboard Readiness

Phase 3 prepares GPUVision for a future React/Next.js dashboard by separating simulation data from presentation formatting. The engine can capture compact timeline events, and `json_export` serializes stable fields that a web UI can consume without scraping console logs.

## Test

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

Google Test is fetched by CMake during configuration.
