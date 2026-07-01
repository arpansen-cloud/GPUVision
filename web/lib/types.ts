export type GpuVisionMode = "demo" | "benchmark";

export interface MetricsSnapshot {
  total_cycles: number;
  instructions_completed: number;
  ipc: number;
  sm_utilization: number;
  idle_cycles: number;
  memory_stalls: number;
  completed_warps: number;
  average_warp_completion_time: number;
  max_warp_completion_time?: number;
  min_warp_completion_time?: number;
  total_sm_cycles?: number;
  non_idle_sm_cycles?: number;
}

export interface SmEvent {
  sm_id: number;
  warp_id: number;
  instruction: "COMPUTE" | "MEMORY" | "IDLE" | string;
  resulting_state: string;
  idle?: boolean;
}

export interface StallEvent {
  warp_id: number;
  before: number;
  after: number;
}

export interface WarpStateSnapshot {
  warp_id: number;
  program_counter: number;
  state: "READY" | "RUNNING" | "STALLED" | "COMPLETED" | string;
  stall_cycles_remaining: number;
}

export interface CycleEvent {
  cycle: number;
  sms: SmEvent[];
  memory_stalls?: StallEvent[];
  completed_warps?: number[];
  warp_states?: WarpStateSnapshot[];
}

export interface DemoRun {
  mode: "demo";
  scheduler: string;
  config: {
    sms: number;
    warps: number;
    instructions_per_warp: number;
    memory_latency: number;
  };
  total_cycles?: number;
  final_metrics: MetricsSnapshot;
  timeline: CycleEvent[];
}

export interface BenchmarkRun {
  mode: "benchmark";
  workload: {
    warps: number;
    instructions_per_warp: number;
    memory_probability: number;
    memory_latency: number;
    sms: number;
    seed: number;
  };
  results: Array<{
    scheduler: string;
    metrics: MetricsSnapshot;
  }>;
}

export type SimulationJson = DemoRun | BenchmarkRun;
