import type { WarpStateSnapshot } from "@/lib/types";
import { stateTone } from "@/lib/format";

interface WarpSummaryProps {
  snapshots: WarpStateSnapshot[];
}

const states = ["READY", "RUNNING", "STALLED", "COMPLETED"];

export function WarpSummary({ snapshots }: WarpSummaryProps) {
  const counts = states.map((state) => ({
    state,
    count: snapshots.filter((snapshot) => snapshot.state === state).length
  }));

  return (
    <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
      <h2 className="mb-4 text-sm font-semibold uppercase tracking-wide text-slate-300">Warp Summary</h2>
      <div className="grid grid-cols-2 gap-3 lg:grid-cols-4">
        {counts.map(({ state, count }) => (
          <div key={state} className="rounded-md border border-line bg-panelSoft p-3">
            <div className={`inline-flex rounded-full px-2.5 py-1 text-xs ${stateTone(state)}`}>{state}</div>
            <div className="mt-3 text-2xl font-semibold text-white">{count}</div>
          </div>
        ))}
      </div>
    </section>
  );
}
