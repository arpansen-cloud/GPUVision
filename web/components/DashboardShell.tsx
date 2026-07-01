"use client";

import { useEffect, useMemo, useState } from "react";
import { Activity, Cpu, Database } from "lucide-react";
import { JsonInputPanel } from "./JsonInputPanel";
import { DemoDashboard } from "./DemoDashboard";
import { BenchmarkDashboard } from "./BenchmarkDashboard";
import { parseSimulationJson } from "@/lib/parseSimulation";
import type { SimulationJson } from "@/lib/types";

const fallbackSample = `{
  "mode": "demo",
  "scheduler": "Round Robin",
  "config": { "sms": 2, "warps": 2, "instructions_per_warp": 2, "memory_latency": 2 },
  "total_cycles": 2,
  "final_metrics": {
    "total_cycles": 2,
    "instructions_completed": 4,
    "ipc": 2,
    "sm_utilization": 100,
    "idle_cycles": 0,
    "memory_stalls": 1,
    "completed_warps": 2,
    "average_warp_completion_time": 2,
    "max_warp_completion_time": 2,
    "min_warp_completion_time": 2,
    "total_sm_cycles": 4,
    "non_idle_sm_cycles": 4
  },
  "timeline": []
}`;

export function DashboardShell() {
  const [rawJson, setRawJson] = useState(fallbackSample);
  const [data, setData] = useState<SimulationJson | null>(() => parseSimulationJson(fallbackSample));
  const [error, setError] = useState<string | null>(null);

  const loadText = (value: string) => {
    setRawJson(value);
  };

  useEffect(() => {
    const id = window.setTimeout(() => {
      try {
        setData(parseSimulationJson(rawJson));
        setError(null);
      } catch (parseError) {
        setError(parseError instanceof Error ? parseError.message : "Unable to parse JSON.");
      }
    }, 150);
    return () => window.clearTimeout(id);
  }, [rawJson]);

  useEffect(() => {
    fetch("/samples/demo.json")
      .then((response) => response.text())
      .then((sample) => setRawJson(sample))
      .catch(() => undefined);
  }, []);

  const summary = useMemo(() => {
    if (!data) {
      return { mode: "No data", title: "Awaiting GPUVision JSON", detail: "Paste or upload a run." };
    }
    if (data.mode === "demo") {
      return {
        mode: "Demo",
        title: data.scheduler,
        detail: `${data.config.sms} SMs · ${data.config.warps} warps · ${data.final_metrics.total_cycles} cycles`
      };
    }
    return {
      mode: "Benchmark",
      title: `${data.results.length} schedulers`,
      detail: `${data.workload.sms} SMs · ${data.workload.warps} warps · seed ${data.workload.seed}`
    };
  }, [data]);

  return (
    <main className="mx-auto flex min-h-screen w-full max-w-[1600px] flex-col gap-5 px-4 py-5 sm:px-6 lg:px-8">
      <header className="grid gap-4 rounded-lg border border-line bg-panel/80 p-5 shadow-glow lg:grid-cols-[1fr_auto] lg:items-center">
        <div>
          <div className="mb-2 flex items-center gap-2 text-sm font-medium text-signal">
            <Cpu size={18} />
            GPUVision
          </div>
          <h1 className="text-3xl font-semibold tracking-normal text-white sm:text-4xl">
            Warp Scheduling Dashboard
          </h1>
        </div>
        <div className="grid gap-3 sm:grid-cols-3">
          <StatusTile icon={<Activity size={18} />} label="Mode" value={summary.mode} />
          <StatusTile icon={<Database size={18} />} label="Run" value={summary.title} />
          <StatusTile icon={<Cpu size={18} />} label="Config" value={summary.detail} />
        </div>
      </header>

      <div className="grid gap-5 2xl:grid-cols-[30rem_minmax(0,1fr)]">
        <JsonInputPanel
          error={error}
          rawJson={rawJson}
          onLoadText={loadText}
          onRawJsonChange={setRawJson}
        />
        <section className="min-w-0">
          {data?.mode === "demo" ? <DemoDashboard data={data} /> : null}
          {data?.mode === "benchmark" ? <BenchmarkDashboard data={data} /> : null}
          {!data ? (
            <div className="rounded-lg border border-line bg-panel/90 p-6 text-slate-300 shadow-glow">
              The dashboard could not render the current input.
            </div>
          ) : null}
        </section>
      </div>
    </main>
  );
}

interface StatusTileProps {
  icon: React.ReactNode;
  label: string;
  value: string;
}

function StatusTile({ icon, label, value }: StatusTileProps) {
  return (
    <div className="rounded-md border border-line bg-panelSoft p-3">
      <div className="flex items-center gap-2 text-xs uppercase tracking-wide text-slate-400">
        {icon}
        {label}
      </div>
      <div className="mt-2 text-sm font-medium text-white">{value}</div>
    </div>
  );
}
