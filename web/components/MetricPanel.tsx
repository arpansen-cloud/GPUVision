import type { MetricsSnapshot } from "@/lib/types";
import { formatNumber } from "@/lib/format";

interface MetricPanelProps {
  metrics: MetricsSnapshot;
}

const metricRows = [
  ["total_cycles", "Total cycles"],
  ["instructions_completed", "Instructions"],
  ["ipc", "IPC"],
  ["sm_utilization", "SM utilization"],
  ["idle_cycles", "Idle cycles"],
  ["memory_stalls", "Memory stalls"],
  ["completed_warps", "Completed warps"],
  ["average_warp_completion_time", "Avg completion"]
] as const;

export function MetricPanel({ metrics }: MetricPanelProps) {
  return (
    <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
      <div className="mb-4 flex items-center justify-between">
        <h2 className="text-sm font-semibold uppercase tracking-wide text-slate-300">Metrics</h2>
        <span className="rounded-full border border-signal/30 bg-signal/10 px-3 py-1 text-xs text-signal">
          {formatNumber(metrics.sm_utilization)}%
        </span>
      </div>
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-4 xl:grid-cols-2">
        {metricRows.map(([key, label]) => {
          const value = metrics[key];
          const suffix = key === "sm_utilization" ? "%" : "";
          return (
            <div key={key} className="rounded-md border border-line bg-panelSoft p-3">
              <div className="text-xs text-slate-400">{label}</div>
              <div className="mt-1 text-xl font-semibold text-white">
                {formatNumber(value)}
                {suffix}
              </div>
            </div>
          );
        })}
      </div>
    </section>
  );
}
