"use client";

import { ChangeEvent, useState } from "react";
import { FileJson, Upload } from "lucide-react";

interface JsonInputPanelProps {
  rawJson: string;
  error: string | null;
  onRawJsonChange: (value: string) => void;
  onLoadText: (value: string) => void;
}

const samples = [
  { label: "Demo sample", path: "/samples/demo.json" },
  { label: "Benchmark sample", path: "/samples/benchmark.json" }
];

export function JsonInputPanel({
  rawJson,
  error,
  onRawJsonChange,
  onLoadText
}: JsonInputPanelProps) {
  const [samplePath, setSamplePath] = useState(samples[0].path);

  const readFile = (event: ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) {
      return;
    }
    const reader = new FileReader();
    reader.onload = () => {
      onLoadText(String(reader.result ?? ""));
    };
    reader.readAsText(file);
  };

  const loadSample = async () => {
    const response = await fetch(samplePath);
    onLoadText(await response.text());
  };

  return (
    <section className="rounded-lg border border-line bg-panel/95 p-4 shadow-glow">
      <div className="mb-3 flex flex-wrap items-center justify-between gap-3">
        <h2 className="text-sm font-semibold uppercase tracking-wide text-slate-300">JSON Input</h2>
        <div className="flex items-center gap-2">
          <select
            className="h-9 rounded-md border border-line bg-panelSoft px-3 text-sm text-slate-200 outline-none focus:border-signal"
            value={samplePath}
            onChange={(event) => setSamplePath(event.target.value)}
          >
            {samples.map((sample) => (
              <option key={sample.path} value={sample.path}>
                {sample.label}
              </option>
            ))}
          </select>
          <button
            className="inline-flex h-9 items-center gap-2 rounded-md border border-signal/40 bg-signal/10 px-3 text-sm text-signal hover:bg-signal/15"
            onClick={loadSample}
            type="button"
          >
            <FileJson size={16} />
            Load
          </button>
          <label className="inline-flex h-9 cursor-pointer items-center gap-2 rounded-md border border-line bg-panelSoft px-3 text-sm text-slate-200 hover:border-signal/60">
            <Upload size={16} />
            Upload
            <input accept="application/json,.json" className="hidden" type="file" onChange={readFile} />
          </label>
        </div>
      </div>
      <textarea
        className="min-h-56 w-full resize-y rounded-md border border-line bg-[#09111c] p-3 font-mono text-xs leading-5 text-slate-200 outline-none focus:border-signal"
        value={rawJson}
        onChange={(event) => onRawJsonChange(event.target.value)}
        spellCheck={false}
      />
      {error ? (
        <div className="mt-3 rounded-md border border-stalled/40 bg-stalled/10 px-3 py-2 text-sm text-stalled">
          {error}
        </div>
      ) : null}
    </section>
  );
}
