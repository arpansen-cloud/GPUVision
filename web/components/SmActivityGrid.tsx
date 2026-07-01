import type { SmEvent } from "@/lib/types";
import { instructionTone } from "@/lib/format";

interface SmActivityGridProps {
  events: SmEvent[];
  expectedSms: number;
}

export function SmActivityGrid({ events, expectedSms }: SmActivityGridProps) {
  const byId = new Map(events.map((event) => [event.sm_id, event]));
  const cards = Array.from({ length: expectedSms }, (_, smId) =>
    byId.get(smId) ?? {
      sm_id: smId,
      warp_id: -1,
      instruction: "IDLE",
      resulting_state: "IDLE",
      idle: true
    }
  );

  return (
    <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
      <div className="mb-4 flex items-center justify-between">
        <h2 className="text-sm font-semibold uppercase tracking-wide text-slate-300">SM Activity</h2>
        <span className="text-xs text-slate-400">{expectedSms} streaming multiprocessors</span>
      </div>
      <div className="grid gap-3 sm:grid-cols-2 xl:grid-cols-4">
        {cards.map((event) => (
          <div key={event.sm_id} className="rounded-lg border border-line bg-panelSoft p-4">
            <div className="flex items-start justify-between gap-3">
              <div>
                <div className="text-xs uppercase tracking-wide text-slate-400">SM {event.sm_id}</div>
                <div className="mt-2 text-2xl font-semibold text-white">
                  {event.idle || event.warp_id < 0 ? "Idle" : `Warp ${event.warp_id}`}
                </div>
              </div>
              <span className={`rounded-full border px-2.5 py-1 text-xs ${instructionTone(event.instruction)}`}>
                {event.instruction}
              </span>
            </div>
            <div className="mt-4 text-sm text-slate-400">State</div>
            <div className="mt-1 font-mono text-sm text-slate-100">{event.resulting_state}</div>
          </div>
        ))}
      </div>
    </section>
  );
}
