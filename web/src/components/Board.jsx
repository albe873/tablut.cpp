import { SIZE } from "../constants";
import { fileLabels } from "../utils";
import { BoardCell } from "./BoardCell";
import { Legend } from "./Legend";

export function Board({
  board,
  selected,
  possibleMoves,
  gameStarted,
  thinking,
  status,
  metrics,
  bestActionValue,
  onCellClick,
}) {
  const rowLabels = Array.from({ length: SIZE }, (_, index) => index + 1);

  return (
    <section className={`board-wrap ${gameStarted ? "" : "board-paused"}`}>
      <div className="board-shell">
        <div className="row-labels" aria-hidden="true">
          {rowLabels.map((label) => (
            <span key={label} className="axis-label row-label">
              {label}
            </span>
          ))}
        </div>

        <div className="board">
          {board.map((piece, i) => {
            const x = i % SIZE;
            const y = Math.floor(i / SIZE);
            const isSelected = selected && selected.x === x && selected.y === y;
            const isMove = possibleMoves.some((m) => m.x === x && m.y === y);

            return (
              <BoardCell
                key={`${x}-${y}`}
                x={x}
                y={y}
                piece={piece}
                isSelected={isSelected}
                isMove={isMove}
                onClick={onCellClick}
              />
            );
          })}
        </div>

        <div className="col-labels" aria-hidden="true">
          {fileLabels.map((label) => (
            <span key={label} className="axis-label col-label">
              {label}
            </span>
          ))}
        </div>
      </div>

      <Legend
        thinking={thinking}
        status={status}
        metrics={metrics}
        bestActionValue={bestActionValue}
      />
    </section>
  );
}
