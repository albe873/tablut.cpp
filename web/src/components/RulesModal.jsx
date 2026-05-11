export function RulesModal({ onClose }) {
  return (
    <div className="modal-backdrop" role="dialog" aria-modal="true" aria-labelledby="rules-title">
      <div className="modal rules-modal">
        <h2 id="rules-title">How to play</h2>

        <section>
          <h3>Goals</h3>
          <p>White wins by moving the king to any edge square.</p>
          <p>Black wins by capturing the king.</p>
        </section>

        <section>
          <h3>Special Squares</h3>
          <ul>
            <li>The throne is the king's starting square in the center.</li>
            <li>Camp squares are black pieces' starting positions.</li>
          </ul>
        </section>

        <section>
          <h3>Movements</h3>
          <ul>
            <li>White moves first.</li>
            <li>Pieces move like a rook in chess: any number of empty normal squares in straight lines.</li>
            <li>If the king leaves the throne, he cannot return in it.</li>
            <li>If a black piece leaves a camp square, it cannot return to a camp square.</li>
          </ul>
        </section>

        <section>
          <h3>Captures</h3>
          <ul>
            <li>A piece is captured when trapped, by an enemy move, between two enemy pieces in opposite directions.</li>
            <li>Throne and camp are active squares, they behave like pieces of the opposite team.</li>
          </ul>
        </section>

        <section>
          <h3>King Capture exceptions</h3>
          <ul>
            <li>The king if is on the throne, to be captured needs to be surrounded by 4 enemy pieces.</li>
            <li>The king if is near the throne, to be captured needs to be surrounded by 3 enemy pieces.</li>
          </ul>
        </section>

        <p className="rules-note">
          Note: this app follows the game logic required by the competition of AI tablut in UniBo. There are a lot of slightly different variants.
        </p>

        <div className="modal-actions">
          <button type="button" className="primary-action" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
}
