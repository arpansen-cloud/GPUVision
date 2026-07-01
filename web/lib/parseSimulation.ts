import type { BenchmarkRun, DemoRun, SimulationJson } from "./types";

const isObject = (value: unknown): value is Record<string, unknown> =>
  typeof value === "object" && value !== null && !Array.isArray(value);

const hasNumber = (value: Record<string, unknown>, key: string): boolean =>
  typeof value[key] === "number" && Number.isFinite(value[key]);

const hasString = (value: Record<string, unknown>, key: string): boolean =>
  typeof value[key] === "string";

function validateDemo(value: unknown): value is DemoRun {
  if (!isObject(value)) {
    return false;
  }
  if (value.mode !== "demo" || !hasString(value, "scheduler")) {
    return false;
  }
  if (!isObject(value.config) || !isObject(value.final_metrics) || !Array.isArray(value.timeline)) {
    return false;
  }
  return (
    hasNumber(value.config, "sms") &&
    hasNumber(value.config, "warps") &&
    hasNumber(value.final_metrics, "total_cycles") &&
    hasNumber(value.final_metrics, "instructions_completed")
  );
}

function validateBenchmark(value: unknown): value is BenchmarkRun {
  if (!isObject(value)) {
    return false;
  }
  if (value.mode !== "benchmark" || !isObject(value.workload) || !Array.isArray(value.results)) {
    return false;
  }
  if (!hasNumber(value.workload, "warps") || !hasNumber(value.workload, "sms")) {
    return false;
  }
  return value.results.every((result) => {
    if (!isObject(result) || !hasString(result, "scheduler") || !isObject(result.metrics)) {
      return false;
    }
    return hasNumber(result.metrics, "total_cycles") && hasNumber(result.metrics, "ipc");
  });
}

export function parseSimulationJson(raw: string): SimulationJson {
  let parsed: unknown;
  try {
    parsed = JSON.parse(raw);
  } catch (error) {
    const message = error instanceof Error ? error.message : "Unknown parser error";
    throw new Error(`Invalid JSON: ${message}`);
  }

  if (validateDemo(parsed)) {
    return parsed;
  }
  if (validateBenchmark(parsed)) {
    return parsed;
  }

  throw new Error("JSON does not match GPUVision demo or benchmark output.");
}
