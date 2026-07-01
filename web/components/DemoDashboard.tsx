"use client";

import { useEffect, useMemo, useState } from "react";
import type { CycleEvent, DemoRun } from "@/lib/types";
import { MetricPanel } from "./MetricPanel";
import { SmActivityGrid } from "./SmActivityGrid";
import { TimelineControls } from "./TimelineControls";
import { TimelineList } from "./TimelineList";
import { WarpSummary } from "./WarpSummary";

interface DemoDashboardProps {
  data: DemoRun;
}

export function DemoDashboard({ data }: DemoDashboardProps) {
  const [cycleIndex, setCycleIndex] = useState(0);
  const [playing, setPlaying] = useState(false);
  const [speedMs, setSpeedMs] = useState(600);
  const maxIndex = Math.max(data.timeline.length - 1, 0);

  useEffect(() => {
    setCycleIndex(0);
    setPlaying(false);
  }, [data]);

  useEffect(() => {
    if (!playing || data.timeline.length === 0) {
      return;
    }
    const id = window.setInterval(() => {
      setCycleIndex((current) => {
        if (current >= maxIndex) {
          return 0;
        }
        return current + 1;
      });
    }, speedMs);
    return () => window.clearInterval(id);
  }, [playing, speedMs, data.timeline.length, maxIndex]);

  const current = data.timeline[cycleIndex] ?? emptyCycle();

  const selectCycle = (cycle: number) => {
    const index = data.timeline.findIndex((entry) => entry.cycle === cycle);
    if (index >= 0) {
      setCycleIndex(index);
    }
  };

  const cycleOptions = useMemo(
    () => ({
      maxCycle: data.timeline[maxIndex]?.cycle ?? 0,
      currentCycle: current.cycle
    }),
    [current.cycle, data.timeline, maxIndex]
  );

  return (
    <div className="grid gap-4 xl:grid-cols-[minmax(0,1fr)_24rem]">
      <div className="space-y-4">
        <TimelineControls
          cycle={cycleOptions.currentCycle}
          maxCycle={cycleOptions.maxCycle}
          playing={playing}
          speedMs={speedMs}
          onCycleChange={selectCycle}
          onPlayPause={() => setPlaying((value) => !value)}
          onReset={() => setCycleIndex(0)}
          onSpeedChange={setSpeedMs}
          onStepBack={() => setCycleIndex((value) => Math.max(0, value - 1))}
          onStepForward={() => setCycleIndex((value) => Math.min(maxIndex, value + 1))}
        />
        <SmActivityGrid events={current.sms} expectedSms={data.config.sms} />
        <WarpSummary snapshots={current.warp_states ?? []} />
        <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
          <div className="flex flex-wrap items-center justify-between gap-3">
            <div>
              <h2 className="text-sm font-semibold uppercase tracking-wide text-slate-300">
                Run Context
              </h2>
              <div className="mt-2 text-2xl font-semibold text-white">{data.scheduler}</div>
            </div>
            <div className="grid grid-cols-2 gap-3 text-sm text-slate-300 sm:grid-cols-4">
              <span>SMs: {data.config.sms}</span>
              <span>Warps: {data.config.warps}</span>
              <span>Instr/warp: {data.config.instructions_per_warp}</span>
              <span>Memory latency: {data.config.memory_latency}</span>
            </div>
          </div>
        </section>
      </div>
      <div className="space-y-4">
        <MetricPanel metrics={data.final_metrics} />
        <TimelineList timeline={data.timeline} currentCycle={current.cycle} onSelectCycle={selectCycle} />
      </div>
    </div>
  );
}

function emptyCycle(): CycleEvent {
  return {
    cycle: 0,
    sms: [],
    memory_stalls: [],
    completed_warps: [],
    warp_states: []
  };
}
