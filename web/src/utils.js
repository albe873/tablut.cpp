import { SIZE, TURN } from "./constants";

export const idx = (x, y) => y * SIZE + x;

export const isCamp = (x, y) => {
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
  return CAMPS_MASK[y]?.[x];
};

export const cellLabel = (x, y) => `${String.fromCharCode(65 + x)}${y + 1}`;

export const fileLabels = Array.from({ length: SIZE }, (_, index) => String.fromCharCode(65 + index));

export const turnLabel = (turn) => {
  if (turn === TURN.WHITE) return "White to move";
  if (turn === TURN.BLACK) return "Black to move";
  if (turn === TURN.WHITE_WIN) return "White wins";
  if (turn === TURN.BLACK_WIN) return "Black wins";
  if (turn === TURN.DRAW) return "Draw";
  return "Unknown";
};

export const isTerminalTurn = (turn) =>
  turn === TURN.WHITE_WIN || turn === TURN.BLACK_WIN || turn === TURN.DRAW;

export const parseMetrics = (metrics) => {
  const parsed = {
    maxDepth: "-",
    nodesExpanded: "-",
    ttMiss: "-",
    ttHit: "-",
  };

  if (!metrics) return parsed;

  for (const rawItem of String(metrics).split(",")) {
    const [rawKey, ...rawValueParts] = rawItem.split(":");
    if (!rawKey || rawValueParts.length === 0) continue;

    const key = rawKey.toLowerCase().replace(/[^a-z]/g, "");
    const value = rawValueParts.join(":").trim() || "-";

    if (key === "maxdepth") parsed.maxDepth = value;
    else if (key === "nodesexpanded" || key === "nodes") parsed.nodesExpanded = value;
    else if (key === "ttmiss") parsed.ttMiss = value;
    else if (key === "tthit") parsed.ttHit = value;
  }

  return parsed;
};

export const playMoveSound = () => {
  const audio = new Audio(
    new URL("../public/sounds-effect/juniorsoundays-ui-sound-101-movepiece.opus", import.meta.url).href
  );
  audio.volume = 0.5;
  audio.play().catch((err) => console.error("Failed to play sound:", err));
};

export const getWasmScriptUrl = () =>
  // Ensure the returned URL is safely encoded to avoid malformed request paths.
  encodeURI(
    new URL(
      "wasm/tablut_wasm.js",
      `${window.location.origin}${import.meta.env.BASE_URL}`
    ).toString()
  );

export const readBoard = (mod, state) => {
  const list = mod.getBoard(state);
  const next = Array(list.size());
  for (let i = 0; i < list.size(); i++) {
    next[i] = list.get(i);
  }
  list.delete();
  return next;
};

export const getMovesForPiece = (mod, state, fromX, fromY) => {
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

export const isSelectablePiece = (turn, piece, PIECE) =>
  (turn === TURN.WHITE && (piece === PIECE.WHITE || piece === PIECE.KING)) ||
  (turn === TURN.BLACK && piece === PIECE.BLACK);
