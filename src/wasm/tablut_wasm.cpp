#include <vector>

#include <emscripten/bind.h>

#include <tablut/game.h>
#include <tablut/actions.h>
#include <tablut/result.h>
#include <tablut/heuristics.h>
#include <adversarialSearch/mtd.h>

using emscripten::register_vector;

namespace wasm_api {

static const Game& getGame() {
    static Game game(State(),
                     Action::getActions,
                     Result::applyAction,
                     Heuristics::getHeuristics,
                     Heuristics::min,
                     Heuristics::max,
                     Heuristics::unknown);
    return game;
}

State createInitialState() {
    return State();
}

std::vector<int> getBoard(const State& state) {
    std::vector<int> out;
    out.reserve(State::size * State::size);
    const Piece (&board)[State::size][State::size] = state.getBoard();
    for (int y = 0; y < State::size; y++) {
        for (int x = 0; x < State::size; x++) {
            out.push_back(static_cast<int>(board[y][x]));
        }
    }
    return out;
}

int getTurn(const State& state) {
    return static_cast<int>(state.getTurn());
}

bool isTerminal(const State& state) {
    Turn turn = state.getTurn();
    return turn == Turn::BlackWin || turn == Turn::WhiteWin || turn == Turn::Draw;
}

std::vector<Move> getPossibleMoves(const State& state) {
    return Action::getActions(state);
}

State applyMove(const State& state, const Move& move) {
    return Result::applyAction(state, move);
}

State applyMoveCoords(const State& state, int fromX, int fromY, int toX, int toY) {
    Move move(cord(fromX, fromY), cord(toX, toY));
    return Result::applyAction(state, move);
}

State createStateFromBoard(const std::vector<int>& flatBoard, int turn) {
    if (flatBoard.size() != State::size * State::size) {
        return State();
    }

    Piece board[State::size][State::size];
    for (int i = 0; i < static_cast<int>(flatBoard.size()); i++) {
        int value = flatBoard[i];
        if (value < static_cast<int>(Piece::Empty) || value > static_cast<int>(Piece::King)) {
            value = static_cast<int>(Piece::Empty);
        }
        const int x = i % State::size;
        const int y = i / State::size;
        board[y][x] = static_cast<Piece>(value);
    }

    return State(board, static_cast<Turn>(turn));
}

struct MoveWithMetrics {
    Move move;
    std::string metrics;
};

Move aiBestMove(const State& state, int maxTimeSeconds) {
    const Game& game = getGame();
    const int tableSize = 2000000;
    mtd<State, Move, Turn, int> search(game, 3, maxTimeSeconds, tableSize);
    return search.makeDecision(state).first;
}

MoveWithMetrics aiBestMoveWithMetrics(const State& state, int maxTimeSeconds) {
    const Game& game = getGame();
    const int tableSize = 2000000;
    mtd<State, Move, Turn, int> search(game, 3, maxTimeSeconds, tableSize);
    auto [move, utility] = search.makeDecision(state);
    return {move, search.getMetrics()};
}

} // namespace wasm_api

EMSCRIPTEN_BINDINGS(tablut_bindings) {
    emscripten::value_object<cord>("Cord")
        .field("x", &cord::x)
        .field("y", &cord::y);

    emscripten::class_<Move>("Move")
        .constructor<>()
        .constructor<cord, cord>()
        .function("getFrom", &Move::getFrom)
        .function("getTo", &Move::getTo);

    emscripten::class_<State>("State")
        .constructor<>()
        .function("getTurn", &State::getTurn)
        .function("boardString", &State::boardString);

    emscripten::value_object<wasm_api::MoveWithMetrics>("MoveWithMetrics")
        .field("move", &wasm_api::MoveWithMetrics::move)
        .field("metrics", &wasm_api::MoveWithMetrics::metrics);

    register_vector<Move>("MoveList");
    register_vector<int>("IntList");

    emscripten::function("createInitialState", &wasm_api::createInitialState);
    emscripten::function("getBoard", &wasm_api::getBoard);
    emscripten::function("getTurn", &wasm_api::getTurn);
    emscripten::function("isTerminal", &wasm_api::isTerminal);
    emscripten::function("getPossibleMoves", &wasm_api::getPossibleMoves);
    emscripten::function("applyMove", &wasm_api::applyMove);
    emscripten::function("applyMoveCoords", &wasm_api::applyMoveCoords);
    emscripten::function("createStateFromBoard", &wasm_api::createStateFromBoard);
    emscripten::function("aiBestMove", &wasm_api::aiBestMove);
    emscripten::function("aiBestMoveWithMetrics", &wasm_api::aiBestMoveWithMetrics);
}
