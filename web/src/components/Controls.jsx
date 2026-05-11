export function DifficultyControl({ difficulty, onDifficultyChange }) {
  return (
    <div>
      <label htmlFor="difficulty-range">Difficulty (ai thinking seconds)</label>
      <div className="control-row">
        <input
          id="difficulty-range"
          type="range"
          min="2"
          max="60"
          value={difficulty}
          onChange={(e) => onDifficultyChange(Number(e.target.value))}
        />
        <input
          aria-label="Difficulty in AI thinking seconds"
          type="number"
          min="2"
          max="60"
          value={difficulty}
          onChange={(e) => onDifficultyChange(Number(e.target.value))}
        />
      </div>
    </div>
  );
}

export function TableSizeControl({ tableSizeScale, onTableSizeChange }) {
  const tableSize = tableSizeScale * 1000000;
  const displayTableSize = `${tableSizeScale}M`;
  // long hash (8) + entry_type (4) + int depth (4) + U score (4) + int best_action_index (4) = 24 bytes per entry
  const memoryBytes = tableSize * 24;
  const memoryMB = (memoryBytes / (1024 * 1024)).toFixed(1);

  return (
    <div>
      <label htmlFor="table-size-range">Transposition Table size</label>
      <div className="control-row">
        <input
          id="table-size-range"
          type="range"
          min="0.01"
          max="10"
          step="0.01"
          value={tableSizeScale}
          onChange={(e) => onTableSizeChange(Number(e.target.value))}
        />
        <span className="control-value">
          {displayTableSize} ({memoryMB} MB)
        </span>
      </div>
    </div>
  );
}
