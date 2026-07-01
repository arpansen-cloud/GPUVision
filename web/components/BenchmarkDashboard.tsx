"use client";

import {
  Bar,
  BarChart,
  CartesianGrid,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis
} from "recharts";
import type { BenchmarkRun } from "@/lib/types";
import { formatNumber } from "@/lib/format";
import { MetricPanel } from "./MetricPanel";

interface BenchmarkDashboardProps {
  data: BenchmarkRun;
}

export function BenchmarkDashboard({ data }: BenchmarkDashboardProps) {
  const chartData = data.results.map((result) => ({
    scheduler: result.scheduler,
    ipc: result.metrics.ipc,
    utilization: result.metrics.sm_utilization,
    cycles: result.metrics.total_cycles
  }));

  const bestIpc = data.results.reduce((best, result) =>
    result.metrics.ipc > best.metrics.ipc ? result : best
  );

  return (
    <div className="space-y-4">
      <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
        <div className="grid gap-4 lg:grid-cols-[1fr_auto] lg:items-center">
          <div>
            <h2 className="text-sm font-semibold uppercase tracking-wide text-slate-300">
              Scheduler Comparison
            </h2>
            <div className="mt-2 text-2xl font-semibold text-white">
              Best IPC: {bestIpc.scheduler}
            </div>
          </div>
          <div className="grid grid-cols-2 gap-3 text-sm text-slate-300 sm:grid-cols-3 lg:grid-cols-6">
            <span>Warps: {data.workload.warps}</span>
            <span>Instr/warp: {data.workload.instructions_per_warp}</span>
            <span>Memory p: {formatNumber(data.workload.memory_probability)}</span>
            <span>Latency: {data.workload.memory_latency}</span>
            <span>SMs: {data.workload.sms}</span>
            <span>Seed: {data.workload.seed}</span>
          </div>
        </div>
      </section>

      <div className="grid gap-4 xl:grid-cols-3">
        {data.results.map((result) => (
          <MetricPanel key={result.scheduler} metrics={result.metrics} />
        ))}
      </div>

      <section className="rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
        <h2 className="mb-4 text-sm font-semibold uppercase tracking-wide text-slate-300">
          Results Table
        </h2>
        <div className="overflow-x-auto">
          <table className="w-full min-w-[52rem] border-collapse text-left text-sm">
            <thead className="text-xs uppercase tracking-wide text-slate-400">
              <tr className="border-b border-line">
                <th className="py-3 pr-4">Scheduler</th>
                <th className="py-3 pr-4">Cycles</th>
                <th className="py-3 pr-4">Instructions</th>
                <th className="py-3 pr-4">IPC</th>
                <th className="py-3 pr-4">SM Utilization</th>
                <th className="py-3 pr-4">Idle</th>
                <th className="py-3 pr-4">Memory stalls</th>
                <th className="py-3 pr-4">Avg completion</th>
              </tr>
            </thead>
            <tbody>
              {data.results.map((result) => (
                <tr key={result.scheduler} className="border-b border-line/70 text-slate-200">
                  <td className="py-3 pr-4 font-medium text-white">{result.scheduler}</td>
                  <td className="py-3 pr-4">{formatNumber(result.metrics.total_cycles)}</td>
                  <td className="py-3 pr-4">{formatNumber(result.metrics.instructions_completed)}</td>
                  <td className="py-3 pr-4">{formatNumber(result.metrics.ipc)}</td>
                  <td className="py-3 pr-4">{formatNumber(result.metrics.sm_utilization)}%</td>
                  <td className="py-3 pr-4">{formatNumber(result.metrics.idle_cycles)}</td>
                  <td className="py-3 pr-4">{formatNumber(result.metrics.memory_stalls)}</td>
                  <td className="py-3 pr-4">
                    {formatNumber(result.metrics.average_warp_completion_time)}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </section>

      <div className="grid gap-4 xl:grid-cols-3">
        <ChartCard title="IPC by Scheduler" data={chartData} dataKey="ipc" color="#6ee7f9" />
        <ChartCard
          title="SM Utilization"
          data={chartData}
          dataKey="utilization"
          color="#34d399"
          suffix="%"
        />
        <ChartCard title="Total Cycles" data={chartData} dataKey="cycles" color="#f59e0b" />
      </div>
    </div>
  );
}

interface ChartCardProps {
  title: string;
  data: Array<Record<string, number | string>>;
  dataKey: string;
  color: string;
  suffix?: string;
}

function ChartCard({ title, data, dataKey, color, suffix = "" }: ChartCardProps) {
  return (
    <section className="h-80 rounded-lg border border-line bg-panel/90 p-4 shadow-glow">
      <h2 className="mb-4 text-sm font-semibold uppercase tracking-wide text-slate-300">{title}</h2>
      <ResponsiveContainer height="100%" width="100%">
        <BarChart data={data} margin={{ bottom: 38, left: 0, right: 6, top: 8 }}>
          <CartesianGrid stroke="#263449" strokeDasharray="3 3" vertical={false} />
          <XAxis
            dataKey="scheduler"
            interval={0}
            stroke="#94a3b8"
            tick={{ fontSize: 11 }}
            tickLine={false}
            angle={-18}
            textAnchor="end"
          />
          <YAxis stroke="#94a3b8" tick={{ fontSize: 11 }} tickLine={false} width={42} />
          <Tooltip
            contentStyle={{
              background: "#101722",
              border: "1px solid #263449",
              borderRadius: 8,
              color: "#e5edf7"
            }}
            formatter={(value) => [`${formatNumber(Number(value))}${suffix}`, title]}
          />
          <Bar dataKey={dataKey} fill={color} radius={[4, 4, 0, 0]} />
        </BarChart>
      </ResponsiveContainer>
    </section>
  );
}
