import type { CycleEvent } from "@/lib/types";
import { instructionTone } from "@/lib/format";

interface TimelineListProps {
  timeline: CycleEvent[];
  currentCycle: number;
  onSelectCycle: (cycle: number) => void;
}

export function TimelineList({ timeline, currentCycle, onSelectCycle }: TimelineListProps) {
  return (
    <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
      <h2 className="mb-4 text-sm font-semibold uppercase tracking-wide text-slate-300">Cycle Log</h2>
      <div className="max-h-[28rem] space-y-2 overflow-y-auto pr-1">
        {timeline.map((cycle) => (
          <button
            key={cycle.cycle}
            className={`w-full rounded-md border p-3 text-left transition ${
              cycle.cycle === currentCycle
                ? "border-signal/60 bg-signal/10"
                : "border-line bg-panelSoft hover:border-signal/40"
            }`}
            onClick={() => onSelectCycle(cycle.cycle)}
            type="button"
          >
            <div className="mb-2 flex items-center justify-between">
              <span className="font-mono text-sm text-white">Cycle {cycle.cycle}</span>
              <span className="text-xs text-slate-400">
                {cycle.completed_warps?.length ?? 0} completed
              </span>
            </div>
            <div className="flex flex-wrap gap-2">
              {cycle.sms.map((event) => (
                <span
                  key={`${cycle.cycle}-${event.sm_id}`}
                  className={`rounded-full border px-2 py-1 text-xs ${instructionTone(event.instruction)}`}
                >
                  SM {event.sm_id}: {event.idle ? "idle" : `W${event.warp_id}`}
                </span>
              ))}
            </div>
          </button>
        ))}
      </div>
    </section>
  );
}
