let wasmPromise = null;
let wasmScriptUrl = null;

const ensureWasm = async () => {
  if (!wasmPromise) {
    if (!wasmScriptUrl) {
      throw new Error("WASM script URL not initialized");
    }
    importScripts(wasmScriptUrl);
    const createModule = self.createTablutModule;
    if (!createModule) {
      throw new Error("createTablutModule not found in worker");
    }
    const wasmBaseUrl = new URL(".", wasmScriptUrl).toString();
    wasmPromise = createModule({
      locateFile: (path) => new URL(path, wasmBaseUrl).toString(),
      mainScriptUrlOrBlob: wasmScriptUrl,
    });
  }
  return wasmPromise;
};

self.onmessage = async (event) => {
  const { type, requestId, board, turn, difficulty, wasmUrl } = event.data || {};

  if (type === "init") {
    if (typeof wasmUrl === "string" && wasmUrl.length > 0) {
      wasmScriptUrl = wasmUrl;
    }
    try {
      await ensureWasm();
    } catch (error) {
      self.postMessage({
        type: "bestMove",
        requestId,
        error: error?.message || "Failed to init AI worker",
      });
    }
    return;
  }

  if (type !== "bestMove") return;

  try {
    const wasm = await ensureWasm();
    // Embind maps std::vector<int> to a JS constructor named "IntList".
    // Create an IntList from the plain JS array so the binding receives the correct type.
    let intList = null;
    try {
      if (wasm.IntList) {
        intList = new wasm.IntList();
        for (let i = 0; i < board.length; i++) {
          intList.push_back(board[i]);
        }
      } else {
        // Fallback: try passing the array directly
        intList = board;
      }
      const state = wasm.createStateFromBoard(intList, turn);
      const moveWithMetrics = wasm.aiBestMoveWithMetrics(state, difficulty);
      const move = moveWithMetrics.move;
      const metrics = moveWithMetrics.metrics;
      const from = move.getFrom();
      const to = move.getTo();
      if (move.delete) {
        move.delete();
      }
      if (state.delete) {
        state.delete();
      }
      if (moveWithMetrics.delete) {
        moveWithMetrics.delete();
      }
      self.postMessage({
        type: "bestMove",
        requestId,
        payload: {
          from: { x: from.x, y: from.y },
          to: { x: to.x, y: to.y },
          metrics,
        },
      });
    } finally {
      if (intList && intList.delete) {
        intList.delete();
      }
    }
  } catch (error) {
    self.postMessage({
      type: "bestMove",
      requestId,
      error: error?.message || "AI move failed",
    });
  }
};
