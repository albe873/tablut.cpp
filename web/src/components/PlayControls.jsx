import { TURN } from "../constants";

export function PlayControls({
  humanSide,
  gameStarted,
  onSideChange,
  onStartGame,
  onResetGame,
}) {
  return (
    <div className="control">
      <label>Play as</label>
      <div className="button-group play-as">
        <button
          type="button"
          className={`play-button white ${humanSide === TURN.WHITE ? "active" : ""}`}
          onClick={() => onSideChange(TURN.WHITE)}
          disabled={gameStarted}
        >
          <span className="play-chip white" aria-hidden="true" />
          White
        </button>
        <button
          type="button"
          className={`play-button black ${humanSide === TURN.BLACK ? "active" : ""}`}
          onClick={() => onSideChange(TURN.BLACK)}
          disabled={gameStarted}
        >
          <span className="play-chip black" aria-hidden="true" />
          Black
        </button>
      </div>
      <label>Game</label>
      <div className="button-group">
        <button
          type="button"
          className="primary-action"
          onClick={onStartGame}
          disabled={gameStarted}
        >
          {gameStarted ? "Game started" : "Play"}
        </button>
        <button type="button" onClick={onResetGame}>
          Reset
        </button>
      </div>
    </div>
  );
}
