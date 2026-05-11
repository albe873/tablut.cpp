import { parseMetrics } from "../utils";

export function Legend({ thinking, status, metrics, bestActionValue }) {
  const parsed = parseMetrics(metrics);
  const metricsItems = [
    `Max Depth: ${parsed.maxDepth}`,
    `Nodes Expanded: ${parsed.nodesExpanded}`,
    `TT Miss: ${parsed.ttMiss}`,
    `TT Hit: ${parsed.ttHit}`,
    `Best Action Value: ${bestActionValue ?? "-"}`,
  ];

  return (
    <aside className="legend">
      <div className="status-card">
        <div className="status-label">Status</div>
        <div className="status-value">{thinking ? "AI thinking..." : status}</div>
      </div>

      <div className="legend-metrics" aria-label="Search metrics">
        <div className="legend-metrics-title">Search Metrics</div>
        {metricsItems.map((item, idx) => (
          <div key={idx} className="legend-metric-item">
            {item}
          </div>
        ))}
      </div>
    </aside>
  );
}
