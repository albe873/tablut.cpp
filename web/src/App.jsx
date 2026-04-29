import { useEffect, useMemo, useRef, useState } from "react";

const SIZE = 9;
const TURN = {
  BLACK: 0,
  WHITE: 1,
  BLACK_WIN: 2,
  WHITE_WIN: 3,
  DRAW: 4,
};

const PIECE = {
  EMPTY: 0,
  BLACK: 1,
  WHITE: 2,
  KING: 3,
};

const CAMPS_MASK = [
  [false, false, false, true, true, true, false, false, false],
  [false, false, false, false, true, false, false, false, false],
  [false, false, false, false, false, false, false, false, false],
  [true, false, false, false, false, false, false, false, true],
  [true, true, false, false, false, false, false, true, true],
  [true, false, false, false, false, false, false, false, true],
  [false, false, false, false, false, false, false, false, false],
  [false, false, false, false, true, false, false, false, false],
  [false, false, false, true, true, true, false, false, false],
];

const THRONE = { x: 4, y: 4 };

const idx = (x, y) => y * SIZE + x;
const isCamp = (x, y) => CAMPS_MASK[y]?.[x];

const turnLabel = (turn) => {
  if (turn === TURN.WHITE) return "White to move";
  if (turn === TURN.BLACK) return "Black to move";
  if (turn === TURN.WHITE_WIN) return "White wins";
  if (turn === TURN.BLACK_WIN) return "Black wins";
  if (turn === TURN.DRAW) return "Draw";
  return "Unknown";
};

const isTerminalTurn = (turn) =>
  turn === TURN.WHITE_WIN || turn === TURN.BLACK_WIN || turn === TURN.DRAW;

export default function App() {
  const [wasm, setWasm] = useState(null);
  const stateRef = useRef(null);
  const aiRequestIdRef = useRef(0);
  const gameStartedRef = useRef(false);
  const aiWorkerRef = useRef(null);
  const aiPendingRef = useRef(new Map());

  const [board, setBoard] = useState(() => Array(SIZE * SIZE).fill(0));
  const [turn, setTurn] = useState(TURN.WHITE);
  const [status, setStatus] = useState("Loading WASM...");
  const [selected, setSelected] = useState(null);
  const [possibleMoves, setPossibleMoves] = useState([]);
  const [difficulty, setDifficulty] = useState(10);
  const [humanSide, setHumanSide] = useState(TURN.WHITE);
  const [thinking, setThinking] = useState(false);
  const [gameOver, setGameOver] = useState(false);
  const [gameOverMessage, setGameOverMessage] = useState("");
  const [gameStarted, setGameStarted] = useState(false);
  const [metrics, setMetrics] = useState(null);
  const [metricsExpanded, setMetricsExpanded] = useState(false);

  const aiSide = useMemo(
    () => (humanSide === TURN.WHITE ? TURN.BLACK : TURN.WHITE),
    [humanSide]
  );

  const getWasmScriptUrl = () =>
    new URL(`wasm/tablut_wasm.js`, `${window.location.origin}${import.meta.env.BASE_URL}`).toString();

  const readBoard = (mod, state) => {
    const list = mod.getBoard(state);
    const next = Array(list.size());
    for (let i = 0; i < list.size(); i++) {
      next[i] = list.get(i);
    }
    list.delete();
    return next;
  };

  const setStateFromWasm = (mod, newState) => {
    if (stateRef.current) {
      stateRef.current.delete();
    }
    stateRef.current = newState;
    const nextBoard = readBoard(mod, newState);
    const nextTurn = mod.getTurn(newState);
    setBoard(nextBoard);
    setTurn(nextTurn);
    setSelected(null);
    setPossibleMoves([]);
    setStatus(turnLabel(nextTurn));
    if (isTerminalTurn(nextTurn)) {
      setGameOver(true);
      setGameOverMessage(turnLabel(nextTurn));
    } else {
      setGameOver(false);
      setGameOverMessage("");
    }
    return nextTurn;
  };

  useEffect(() => {
    gameStartedRef.current = gameStarted;
  }, [gameStarted]);

  const initAiWorker = () => {
    if (aiWorkerRef.current) {
      aiWorkerRef.current.terminate();
    }
    const worker = new Worker(new URL("./aiWorker.js", import.meta.url));
    aiWorkerRef.current = worker;
    for (const pending of aiPendingRef.current.values()) {
      pending.reject(new Error("AI request canceled"));
    }
    aiPendingRef.current.clear();

    worker.onmessage = (event) => {
      const { type, requestId, payload, error } = event.data || {};
      if (type === "bestMove") {
        const pending = aiPendingRef.current.get(requestId);
        if (pending) {
          aiPendingRef.current.delete(requestId);
          if (error) {
            pending.reject(new Error(error));
          } else {
            pending.resolve(payload);
          }
        }
      }
    };

    worker.postMessage({ type: "init", wasmUrl: getWasmScriptUrl() });
  };

  useEffect(() => {
    initAiWorker();
    return () => {
      if (aiWorkerRef.current) {
        aiWorkerRef.current.terminate();
        aiWorkerRef.current = null;
      }
    };
  }, []);

  const cancelAi = () => {
    aiRequestIdRef.current += 1;
    setThinking(false);
    initAiWorker();
  };

  const resetGame = () => {
    if (!wasm) return;
    cancelAi();
    setGameOver(false);
    setGameOverMessage("");
    setGameStarted(false);
    setMetrics(null);
    setMetricsExpanded(false);
    const nextState = wasm.createInitialState();
    setStateFromWasm(wasm, nextState);
  };

  const getMovesForPiece = (mod, state, fromX, fromY) => {
    const list = mod.getPossibleMoves(state);
    const out = [];
    for (let i = 0; i < list.size(); i++) {
      const move = list.get(i);
      const from = move.getFrom();
      if (from.x === fromX && from.y === fromY) {
        const to = move.getTo();
        out.push({ x: to.x, y: to.y });
      }
      if (move.delete) {
        move.delete();
      }
    }
    list.delete();
    return out;
  };

  const maybeTriggerAi = async (nextTurn) => {
    if (!wasm || !stateRef.current) return;
    if (thinking) return;
    if (!gameStartedRef.current) return;
    if (isTerminalTurn(nextTurn)) return;
    if (nextTurn !== aiSide) return;

    const requestId = (aiRequestIdRef.current += 1);
    setThinking(true);
    await new Promise((resolve) => setTimeout(resolve, 10));

    try {
      const boardSnapshot = readBoard(wasm, stateRef.current);
      const bestMove = await new Promise((resolve, reject) => {
        aiPendingRef.current.set(requestId, { resolve, reject });
        aiWorkerRef.current?.postMessage({
          type: "bestMove",
          requestId,
          board: boardSnapshot,
          turn: nextTurn,
          difficulty,
        });
      });
      const { from, to, metrics: moveMetrics } = bestMove;
      if (moveMetrics) {
        setMetrics(moveMetrics);
      }
      const newState = wasm.applyMoveCoords(
        stateRef.current,
        from.x,
        from.y,
        to.x,
        to.y
      );
      if (requestId !== aiRequestIdRef.current || !gameStartedRef.current) {
        if (newState?.delete) {
          newState.delete();
        }
        return null;
      }
      const updatedTurn = setStateFromWasm(wasm, newState);
      return updatedTurn;
    } catch (error) {
      if (error?.message === "AI request canceled") {
        return null;
      }
      console.error("AI move failed", error);
      setStatus("AI error - check console");
      return null;
    } finally {
      if (requestId === aiRequestIdRef.current) {
        setThinking(false);
      }
    }
  };

  const handleCellClick = async (x, y) => {
    if (!wasm || !stateRef.current) return;
    if (thinking) return;
    if (isTerminalTurn(turn)) return;
    if (turn !== humanSide) return;

    const isDestination = possibleMoves.some((m) => m.x === x && m.y === y);
    if (selected && isDestination) {
      const newState = wasm.applyMoveCoords(
        stateRef.current,
        selected.x,
        selected.y,
        x,
        y
      );
      const updatedTurn = setStateFromWasm(wasm, newState);
      await maybeTriggerAi(updatedTurn);
      return;
    }

    const piece = board[idx(x, y)];
    const isSelectable =
      (turn === TURN.WHITE && (piece === PIECE.WHITE || piece === PIECE.KING)) ||
      (turn === TURN.BLACK && piece === PIECE.BLACK);

    if (!isSelectable) {
      setSelected(null);
      setPossibleMoves([]);
      return;
    }

    const moves = getMovesForPiece(wasm, stateRef.current, x, y);
    setSelected({ x, y });
    setPossibleMoves(moves);
  };

  useEffect(() => {
    let isMounted = true;

    const loadScript = (url) =>
      new Promise((resolve, reject) => {
        const existing = document.querySelector(`script[src="${url}"]`);
        if (existing) {
          existing.addEventListener("load", resolve, { once: true });
          existing.addEventListener("error", reject, { once: true });
          if (existing.dataset.loaded === "true") resolve();
          return;
        }
        const script = document.createElement("script");
        script.src = url;
        script.async = true;
        script.onload = () => {
          script.dataset.loaded = "true";
          resolve();
        };
        script.onerror = reject;
        document.body.appendChild(script);
      });

    const loadWasm = async () => {
      try {
        const wasmScriptUrl = getWasmScriptUrl();
        await loadScript(wasmScriptUrl);
        const createModule = globalThis.createTablutModule;
        if (!createModule) {
          throw new Error("createTablutModule not found on window");
        }
        const instance = await createModule({
          locateFile: (path) => new URL(path, new URL(".", wasmScriptUrl)).toString(),
          mainScriptUrlOrBlob: wasmScriptUrl,
        });
        if (!isMounted) return;
        setWasm(instance);
        const initState = instance.createInitialState();
        const nextTurn = setStateFromWasm(instance, initState);
        maybeTriggerAi(nextTurn);
      } catch (error) {
        console.error("WASM load failed", error);
        setStatus("WASM load failed - check console");
      }
    };
    loadWasm();
    return () => {
      isMounted = false;
      if (stateRef.current) {
        stateRef.current.delete();
        stateRef.current = null;
      }
    };
  }, []);

  useEffect(() => {
    if (!wasm || !stateRef.current) return;
    setStatus(turnLabel(turn));
  }, [turn, wasm]);

  useEffect(() => {
    if (!wasm || !stateRef.current) return;
    if (gameStarted && turn !== humanSide) {
      maybeTriggerAi(turn);
    }
  }, [humanSide, gameStarted]);

  const handleSideChange = (side) => {
    if (side === humanSide) return;
    // Prevent changing side while a game is in progress
    if (gameStartedRef.current) return;
    setHumanSide(side);
  };

  const handleStartGame = () => {
    if (!wasm) return;
    setGameStarted(true);
    if (turn !== humanSide) {
      maybeTriggerAi(turn);
    }
  };

  return (
    <div className="page">
      <header className="hero">
        <div>
          <h1>Tablut.cpp web</h1>
          <div className="hero-meta">
            <span className="subtitle">Play against engine compiled to WebAssembly.</span>
            <span className="hero-sep">|</span>
            <a
              className="rules-link"
              href="https://en.wikipedia.org/wiki/Tafl_games#Tablut"
              target="_blank"
              rel="noopener noreferrer"
            >
              Rules and other information about Tablut
            </a>
          </div>
        </div>
      </header>

      <section className="controls">
        <div className="control">
          <label>Difficulty (ai thinking seconds)</label>
          <div className="control-row">
            <input
              type="range"
              min="5"
              max="60"
              value={difficulty}
              onChange={(e) => setDifficulty(Number(e.target.value))}
            />
            <input
              type="number"
              min="5"
              max="60"
              value={difficulty}
              onChange={(e) => setDifficulty(Number(e.target.value))}
            />
          </div>
        </div>

        <div className="control">
          <label>Play as</label>
          <div className="button-group play-as">
            <button
              type="button"
              className={`play-button white ${humanSide === TURN.WHITE ? "active" : ""}`}
              onClick={() => handleSideChange(TURN.WHITE)}
              disabled={gameStarted}
            >
              <span className="play-chip white" aria-hidden="true" />
              White
            </button>
            <button
              type="button"
              className={`play-button black ${humanSide === TURN.BLACK ? "active" : ""}`}
              onClick={() => handleSideChange(TURN.BLACK)}
              disabled={gameStarted}
            >
              <span className="play-chip black" aria-hidden="true" />
              Black
            </button>
          </div>
          <div className="play-selected">
            Selected:
            <span className={`play-chip ${humanSide === TURN.WHITE ? "white" : "black"}`} />
            {humanSide === TURN.WHITE ? "White" : "Black"}
          </div>
        </div>

        <div className="control">
          <label>Game</label>
          <div className="button-group">
            <button
              type="button"
              className="primary-action"
              onClick={handleStartGame}
              disabled={gameStarted}
            >
              {gameStarted ? "Game started" : "Play"}
            </button>
            <button type="button" onClick={resetGame}>
              Reset
            </button>
          </div>
        </div>
      </section>

      <section className={`board-wrap ${gameStarted ? "" : "board-paused"}`}>
        <div className="board">
          {board.map((piece, i) => {
            const x = i % SIZE;
            const y = Math.floor(i / SIZE);
            const isSelected = selected && selected.x === x && selected.y === y;
            const isMove = possibleMoves.some((m) => m.x === x && m.y === y);
            const isThrone = THRONE.x === x && THRONE.y === y;

            return (
              <button
                key={`${x}-${y}`}
                className={`cell ${
                  isCamp(x, y) ? "camp" : ""
                } ${isThrone ? "throne" : ""} ${
                  isSelected ? "selected" : ""
                } ${isMove ? "move" : ""}`}
                onClick={() => handleCellClick(x, y)}
                type="button"
              >
                {piece !== PIECE.EMPTY && (
                  <span className={`piece ${
                    piece === PIECE.BLACK
                      ? "black"
                      : piece === PIECE.WHITE
                      ? "white"
                      : "king"
                  }`}>
                    {piece === PIECE.KING ? "K" : ""}
                  </span>
                )}
              </button>
            );
          })}
        </div>

        <aside className="legend">
          <div className="status-card status-inline">
            <div className="status-label">Status</div>
            <div className="status-value">{thinking ? "AI thinking..." : status}</div>
            <div className="status-sub">
              You are playing {humanSide === TURN.WHITE ? "White" : "Black"}
            </div>
          </div>
          
          {metrics && (
            <div className="metrics-card">
              <button 
                className="metrics-toggle"
                onClick={() => setMetricsExpanded(!metricsExpanded)}
                type="button"
              >
                <span className="metrics-label">Search Metrics</span>
                <span className="metrics-arrow">{"▼"}</span>
              </button>
              {metricsExpanded && (
                <div className="metrics-content">
                  {metrics.split(", ").map((item, idx) => (
                    <div key={idx} className="metric-item">
                      {item}
                    </div>
                  ))}
                </div>
              )}
            </div>
          )}
          
          <h3>Legend</h3>
          <div className="legend-item">
            <span className="piece white" /> White piece
          </div>
          <div className="legend-item">
            <span className="piece black" /> Black piece
          </div>
          <div className="legend-item">
            <span className="piece king">K</span> King
          </div>
          <div className="legend-item camp-chip">Camp cell</div>
          <div className="legend-item throne-chip">Throne cell</div>
        </aside>
      </section>

      {gameOver && (
        <div className="modal-backdrop" role="dialog" aria-modal="true">
          <div className="modal">
            <p className="modal-eyebrow">Game over</p>
            <h2>{gameOverMessage}</h2>
            <p>Play again or review the final position on the board.</p>
            <div className="modal-actions">
              <button type="button" onClick={resetGame}>
                Play again
              </button>
              <button type="button" onClick={() => setGameOver(false)}>
                Close
              </button>
            </div>
          </div>
        </div>
      )}

      <footer className="footer">
        <span>Tip: select a piece to see legal moves.</span>
        <a
          className="footer-link"
          href="https://github.com/albe873/tablut.cpp"
          target="_blank"
          rel="noopener noreferrer"
        >
          By albe873 - View on GitHub
        </a>
      </footer>
    </div>
  );
}
