import { useEffect, useMemo, useRef, useState, useCallback } from "react";
import {
  Header,
  DifficultyControl,
  TableSizeControl,
  PlayControls,
  Board,
  GameOverModal,
  RulesModal,
  Footer,
} from "./components";
import { SIZE, TURN, PIECE } from "./constants";
import {
  idx,
  turnLabel,
  isTerminalTurn,
  playMoveSound,
  getWasmScriptUrl,
  readBoard,
  getMovesForPiece,
  isSelectablePiece,
} from "./utils";

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
  const [difficulty, setDifficulty] = useState(4);
  const [tableSizeScale, setTableSizeScale] = useState(1);
  const [humanSide, setHumanSide] = useState(TURN.WHITE);

  const tableSize = tableSizeScale * 1000000;
  
  const [thinking, setThinking] = useState(false);
  const [gameOver, setGameOver] = useState(false);
  const [gameOverMessage, setGameOverMessage] = useState("");
  const [showRules, setShowRules] = useState(false);
  const [gameStarted, setGameStarted] = useState(false);
  const [metrics, setMetrics] = useState(null);
  const [bestActionValue, setBestActionValue] = useState(null);

  const aiSide = useMemo(
    () => (humanSide === TURN.WHITE ? TURN.BLACK : TURN.WHITE),
    [humanSide]
  );

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
    setBestActionValue(null);
    const nextState = wasm.createInitialState();
    setStateFromWasm(wasm, nextState);
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
          tableSize,
        });
      });
      const { from, to, metrics: moveMetrics, utility } = bestMove;
      if (moveMetrics) {
        setMetrics(moveMetrics);
      }
      if (typeof utility !== "undefined") {
        setBestActionValue(String(utility));
      }
      const newState = wasm.applyMoveCoords(
        stateRef.current,
        from.x,
        from.y,
        to.x,
        to.y
      );
      playMoveSound();
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

  const handleCellClick = useCallback(async (x, y) => {
    if (!wasm || !stateRef.current) return;
    if (thinking) return;
    if (isTerminalTurn(turn)) return;
    if (turn !== humanSide) return;
    if (!gameStarted) return;

    const isDestination = possibleMoves.some((m) => m.x === x && m.y === y);
    if (selected && isDestination) {
      const newState = wasm.applyMoveCoords(
        stateRef.current,
        selected.x,
        selected.y,
        x,
        y
      );
      playMoveSound();
      const updatedTurn = setStateFromWasm(wasm, newState);
      await maybeTriggerAi(updatedTurn);
      return;
    }

    const piece = board[idx(x, y)];
    const isSelectable = isSelectablePiece(turn, piece, PIECE);

    if (!isSelectable) {
      setSelected(null);
      setPossibleMoves([]);
      return;
    }

    const moves = getMovesForPiece(wasm, stateRef.current, x, y);
    setSelected({ x, y });
    setPossibleMoves(moves);
  }, [wasm, thinking, turn, humanSide, gameStarted, possibleMoves, selected, board]);

  useEffect(() => {
    let isMounted = true;

    const loadScript = (url) =>
      new Promise((resolve, reject) => {
        const safeUrl = encodeURI(String(url));
        const existing = document.querySelector(`script[src="${safeUrl}"]`);
        if (existing) {
          existing.addEventListener("load", resolve, { once: true });
          existing.addEventListener("error", reject, { once: true });
          if (existing.dataset.loaded === "true") resolve();
          return;
        }
        const script = document.createElement("script");
        script.src = safeUrl;
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

  const handleSideChange = useCallback((side) => {
    if (side === humanSide) return;
    // Prevent changing side while a game is in progress
    if (gameStartedRef.current) return;
    setHumanSide(side);
  }, [humanSide]);

  const startGame = useCallback(() => {
    if (!wasm) return;
    setGameStarted(true);
    if (turn !== humanSide) {
      maybeTriggerAi(turn);
    }
  }, [wasm, turn, humanSide]);

  const restartGame = () => {
    resetGame();
    startGame();
  };

  return (
    <div className="page">
      <Header onShowRules={() => setShowRules(true)} />

      <section className="controls">
        <div className="control">
          <DifficultyControl difficulty={difficulty} onDifficultyChange={setDifficulty} />
          <TableSizeControl
            tableSizeScale={tableSizeScale}
            onTableSizeChange={setTableSizeScale}
          />
        </div>

        <PlayControls
          humanSide={humanSide}
          gameStarted={gameStarted}
          onSideChange={handleSideChange}
          onStartGame={startGame}
          onResetGame={resetGame}
        />
      </section>

      <Board
        board={board}
        selected={selected}
        possibleMoves={possibleMoves}
        gameStarted={gameStarted}
        thinking={thinking}
        status={status}
        metrics={metrics}
        bestActionValue={bestActionValue}
        onCellClick={handleCellClick}
      />

      {gameOver && (
        <GameOverModal
          gameOverMessage={gameOverMessage}
          onPlayAgain={restartGame}
          onClose={() => setGameOver(false)}
        />
      )}

      {showRules && <RulesModal onClose={() => setShowRules(false)} />}

      <Footer />
    </div>
  );
}
