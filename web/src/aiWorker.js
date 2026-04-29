let wasmPromise = null;
let baseUrl = "/";

const ensureWasm = async () => {
  if (!wasmPromise) {
    importScripts(`${baseUrl}wasm/tablut_wasm.js`);
    const createModule = self.createTablutModule;
    if (!createModule) {
      throw new Error("createTablutModule not found in worker");
    }
    wasmPromise = createModule({
      locateFile: (path) => `${baseUrl}wasm/${path}`,
      mainScriptUrlOrBlob: new URL(`${baseUrl}wasm/tablut_wasm.js`, location.href).toString(),
    });
  }
  return wasmPromise;
};

self.onmessage = async (event) => {
  const { type, requestId, board, turn, difficulty, basePath } = event.data || {};

  if (type === "init") {
    if (typeof basePath === "string" && basePath.length > 0) {
      baseUrl = basePath.endsWith("/") ? basePath : `${basePath}/`;
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
