export function Header({ onShowRules }) {
  return (
    <header className="hero">
      <div>
        <h1>Tablut.cpp web</h1>
        <div className="hero-meta">
          <span className="subtitle">Play against the AI engine.</span>
          <button
            type="button"
            className="rules-link"
            onClick={onShowRules}
          >
            Rules
          </button>
        </div>
      </div>
    </header>
  );
}
