# GPUVision

GPUVision Phase 1 is a C++17 command-line simulator for GPU warp scheduling across Streaming Multiprocessors (SMs).

## What It Models

- A warp has an ID, program counter, state, stall counter, and instruction stream.
- Instructions are `COMPUTE` or `MEMORY`.
- `COMPUTE` completes in one cycle.
- `MEMORY` completes as an issued instruction, then stalls the warp for the configured memory latency.
- Each SM can issue one instruction per cycle.
- If no warp is ready, the SM records an idle cycle.
- Completed warps are never scheduled again.

## Schedulers

- Round Robin
- Oldest First
- Greedy Then Oldest

## Metrics

- Total cycles
- Instructions completed
- IPC
- SM utilization percentage
- Idle cycles
- Memory stall count
- Completed warps

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/gpuvision
```

The demo run creates 8 warps, uses 2 SMs, prints cycle-by-cycle scheduling logs, and then prints final metrics.

## Test

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

Google Test is fetched by CMake during configuration.

## Phase Scope

This repository intentionally implements Phase 1 only: the C++ command-line simulator. JSON export and frontend work are out of scope for this phase.
