# GPUVision Dashboard

The Phase 4 dashboard is a Next.js interface for JSON files produced by the C++ GPUVision simulator. It does not run the simulator itself; it reads exported JSON and visualizes scheduling behavior.

## Install

```sh
cd web
npm install
```

## Run

```sh
npm run dev
```

Open the local URL printed by Next.js, usually `http://localhost:3000`.

## Expected JSON

Generate demo JSON from the C++ simulator:

```sh
./build/gpuvision --demo --json --export demo.json
```

Generate benchmark JSON:

```sh
./build/gpuvision --benchmark --json \
  --warps 64 \
  --instructions 50 \
  --memory-probability 0.25 \
  --memory-latency 20 \
  --sms 4 \
  --seed 42 \
  --export benchmark.json
```

The dashboard accepts:

- Demo JSON with `mode: "demo"`, `final_metrics`, and `timeline`
- Benchmark JSON with `mode: "benchmark"`, `workload`, and `results`

Sample files are included in `public/samples/`.

## Features

- Upload a GPUVision JSON file
- Paste JSON directly
- Load built-in demo or benchmark samples
- Play, pause, step, reset, and scrub demo timelines
- Inspect current SM activity and warp states per cycle
- Review final metrics for demo runs
- Compare benchmark schedulers in tables and charts
- Chart IPC, SM utilization, and total cycles by scheduler

Invalid JSON is handled in the input panel without clearing the last valid dashboard state.
