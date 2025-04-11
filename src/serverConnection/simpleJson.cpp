// simpleJson.cpp

#include "serverConnection/simpleJson.h"

std::string cordToString(cord c) {
    // Convert x (0-8) to letters 'a' through 'i'
    char column = 'a' + c.x;
    // Convert y (0-8) to numbers 1-9
    std::string row = std::to_string(c.y + 1);
    return std::string(1, column) + row;
}

std::string turnToString(const Turn& turn) {
    switch (turn) {
        case Turn::Black: return "BLACK";
        case Turn::White: return "WHITE";
        default: return "UNKNOWN";
    }
}

std::string SimpleJson::toJson(const std::string& name) {
    std::string json = "\"";
    json += name;
    json += "\"";
    return json;
}

std::string SimpleJson::toJson(const Move& move, const Turn& turn) {
    std::string json = "{";
    json += "\"from\":\"" + cordToString(move.getFrom()) + "\",";
    json += "\"to\":\"" + cordToString(move.getTo()) + "\",";
    json += "\"turn\":\"" + turnToString(turn) + "\"";
    json += "}";
    return json;
}

State SimpleJson::fromJson(std::string json) {
    // 1) Replace "THRONE" with "EMPTY"
    std::string throneStr = "\"THRONE\"";
    std::string emptyStr = "\"EMPTY\"";
    size_t startPos = 0;
    while((startPos = json.find(throneStr, startPos)) != std::string::npos) {
        json.replace(startPos, throneStr.size(), emptyStr);
        startPos += emptyStr.size();
    }

    // 2) Extract turn
    Turn turn = Turn::Black; // default
    {
        size_t posTurn = json.find("\"turn\"");
        if (posTurn != std::string::npos) {
            size_t startQuote = json.find("\"", posTurn + 6);
            size_t endQuote = json.find("\"", startQuote + 1);
            std::string turnValue = json.substr(startQuote + 1, endQuote - startQuote - 1);
            if (turnValue == "BLACK") turn = Turn::Black;
            else if (turnValue == "WHITE") turn = Turn::White;
            else if (turnValue == "BLACKWIN") turn = Turn::BlackWin;
            else if (turnValue == "WHITEWIN") turn = Turn::WhiteWin;
            else if (turnValue == "DRAW") turn = Turn::Draw;
        }
    }

    // 3) Extract board (naive substring parsing)
    Piece newBoard[State::size][State::size];
    {
        size_t boardPos = json.find("\"board\"");
        if (boardPos != std::string::npos) {
            // Move to first '[' after "board":
            size_t arrayStart = json.find('[', boardPos);
            size_t rowStart = arrayStart + 1;
            for (int r = 0; r < State::size; r++) {
                // Find the row slice
                size_t rowOpen = json.find('[', rowStart);
                size_t rowClose = json.find(']', rowOpen);
                std::string rowString = json.substr(rowOpen + 1, rowClose - rowOpen - 1);

                // Split by commas to get cell strings
                // Each cell is like "EMPTY" or "BLACK"
                int col = 0;
                size_t cellStart = 0;
                while (col < State::size && cellStart < rowString.size()) {
                    // Find next quote
                    size_t quote1 = rowString.find('"', cellStart);
                    size_t quote2 = rowString.find('"', quote1 + 1);
                    std::string cellVal = "";
                    if (quote1 != std::string::npos && quote2 != std::string::npos) {
                        cellVal = rowString.substr(quote1 + 1, quote2 - quote1 - 1);
                    }
                    if (cellVal == "EMPTY") newBoard[r][col] = Piece::Empty;
                    else if (cellVal == "BLACK") newBoard[r][col] = Piece::Black;
                    else if (cellVal == "WHITE") newBoard[r][col] = Piece::White;
                    else if (cellVal == "KING") newBoard[r][col] = Piece::King;
                    else newBoard[r][col] = Piece::Empty;
                    cellStart = quote2 + 1;
                    col++;
                }
                rowStart = rowClose + 1;
            }
        }
    }

    State parsedState(newBoard, turn);
    return parsedState;
}
