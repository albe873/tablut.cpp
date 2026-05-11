import { memo } from "react";
import { PIECE, THRONE } from "../constants";
import { cellLabel, isCamp } from "../utils";

export const BoardCell = memo(({ x, y, piece, isSelected, isMove, onClick }) => {
  const isThrone = THRONE.x === x && THRONE.y === y;
  
  return (
    <button
      className={`cell ${isCamp(x, y) ? "camp" : ""} ${isThrone ? "throne" : ""} ${
        isSelected ? "selected" : ""
      } ${isMove ? "move" : ""}`}
      title={cellLabel(x, y)}
      onClick={() => onClick(x, y)}
      type="button"
    >
      {piece !== PIECE.EMPTY && (
        <span
          className={`piece ${
            piece === PIECE.BLACK
              ? "black"
              : piece === PIECE.WHITE
              ? "white"
              : "king"
          }`}
        >
          {piece === PIECE.KING ? "K" : ""}
        </span>
      )}
    </button>
  );
});

BoardCell.displayName = "BoardCell";
