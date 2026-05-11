export function GameOverModal({ gameOverMessage, onPlayAgain, onClose }) {
  return (
    <div className="modal-backdrop" role="dialog" aria-modal="true">
      <div className="modal">
        <p className="modal-eyebrow">Game over</p>
        <h2>{gameOverMessage}</h2>
        <p>Play again or review the final position on the board.</p>
        <div className="modal-actions">
          <button type="button" className="primary-action" onClick={onPlayAgain}>
            Play again
          </button>
          <button type="button" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
}
