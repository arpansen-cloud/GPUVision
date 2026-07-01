import type { Config } from "tailwindcss";

const config: Config = {
  content: [
    "./app/**/*.{js,ts,jsx,tsx,mdx}",
    "./components/**/*.{js,ts,jsx,tsx,mdx}",
    "./lib/**/*.{js,ts,jsx,tsx,mdx}"
  ],
  theme: {
    extend: {
      colors: {
        panel: "#101722",
        panelSoft: "#162031",
        line: "#263449",
        signal: "#6ee7f9",
        compute: "#34d399",
        memory: "#f59e0b",
        idle: "#64748b",
        stalled: "#fb7185"
      },
      boxShadow: {
        glow: "0 0 0 1px rgba(110, 231, 249, 0.12), 0 20px 80px rgba(0, 0, 0, 0.35)"
      }
    }
  },
  plugins: []
};

export default config;
