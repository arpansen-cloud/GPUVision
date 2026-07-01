"use client";

import { Pause, Play, RotateCcw, SkipBack, SkipForward } from "lucide-react";

interface TimelineControlsProps {
  cycle: number;
  maxCycle: number;
  playing: boolean;
  speedMs: number;
  onPlayPause: () => void;
  onStepBack: () => void;
  onStepForward: () => void;
  onReset: () => void;
  onCycleChange: (value: number) => void;
  onSpeedChange: (value: number) => void;
}

export function TimelineControls({
  cycle,
  maxCycle,
  playing,
  speedMs,
  onPlayPause,
  onStepBack,
  onStepForward,
  onReset,
  onCycleChange,
  onSpeedChange
}: TimelineControlsProps) {
  return (
    <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
      <div className="mb-4 flex flex-wrap items-center justify-between gap-3">
        <div>
          <h2 className="text-sm font-semibold uppercase tracking-wide text-slate-300">Timeline</h2>
          <div className="mt-1 text-3xl font-semibold text-white">
            Cycle {cycle}
            <span className="text-base font-normal text-slate-400"> / {maxCycle}</span>
          </div>
        </div>
        <div className="flex items-center gap-2">
          <button className="rounded-md border border-line bg-panelSoft p-2 text-slate-200 hover:border-signal/60" onClick={onReset} title="Reset" type="button">
            <RotateCcw size={18} />
          </button>
          <button className="rounded-md border border-line bg-panelSoft p-2 text-slate-200 hover:border-signal/60" onClick={onStepBack} title="Step backward" type="button">
            <SkipBack size={18} />
          </button>
          <button className="rounded-md border border-signal/40 bg-signal/10 p-2 text-signal hover:bg-signal/15" onClick={onPlayPause} title={playing ? "Pause" : "Play"} type="button">
            {playing ? <Pause size={18} /> : <Play size={18} />}
          </button>
          <button className="rounded-md border border-line bg-panelSoft p-2 text-slate-200 hover:border-signal/60" onClick={onStepForward} title="Step forward" type="button">
            <SkipForward size={18} />
          </button>
        </div>
      </div>
      <div className="grid gap-3 sm:grid-cols-[1fr_auto] sm:items-center">
        <input
          aria-label="Current cycle"
          className="w-full accent-signal"
          max={maxCycle}
          min={0}
          type="range"
          value={cycle}
          onChange={(event) => onCycleChange(Number(event.target.value))}
        />
        <label className="flex items-center gap-2 text-sm text-slate-400">
          Speed
          <select
            className="rounded-md border border-line bg-panelSoft px-2 py-1 text-slate-200"
            value={speedMs}
            onChange={(event) => onSpeedChange(Number(event.target.value))}
          >
            <option value={900}>0.75x</option>
            <option value={600}>1x</option>
            <option value={300}>2x</option>
            <option value={150}>4x</option>
          </select>
        </label>
      </div>
    </section>
  );
}
