export function formatNumber(value: number | undefined, digits = 2): string {
  if (typeof value !== "number" || !Number.isFinite(value)) {
    return "0";
  }
  if (Number.isInteger(value)) {
    return value.toLocaleString();
  }
  return value.toLocaleString(undefined, {
    minimumFractionDigits: digits,
    maximumFractionDigits: digits
  });
}

export function instructionTone(instruction: string): string {
  if (instruction === "COMPUTE") {
    return "border-compute/40 bg-compute/10 text-compute";
  }
  if (instruction === "MEMORY") {
    return "border-memory/40 bg-memory/10 text-memory";
  }
  return "border-idle/40 bg-idle/10 text-slate-300";
}

export function stateTone(state: string): string {
  if (state === "READY") {
    return "bg-signal/15 text-signal";
  }
  if (state === "RUNNING") {
    return "bg-compute/15 text-compute";
  }
  if (state === "STALLED") {
    return "bg-stalled/15 text-stalled";
  }
  if (state === "COMPLETED") {
    return "bg-emerald-500/15 text-emerald-300";
  }
  return "bg-slate-500/15 text-slate-300";
}
