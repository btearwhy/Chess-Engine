#include "Engine.hpp"

std::optional<HashInfo> Engine::hashInfo() const {
    return std::nullopt;
}

void Engine::setHashSize(std::size_t) {}


void MyEngine::newGame() {
    return;
};

const int MyEngine::SEARCH_DEPTH = 5;


PrincipalVariation MyEngine::pv(
    const Board& board,
    const TimeInfo::Optional& timeInfo = std::nullopt
) {
    
    (void) timeInfo;
    PrincipalVariation pv_;
    Board board_s = board;
    if (board.turn() == PieceColor::White) {
        pv_ = minimax(board_s, INT_MIN, INT_MAX, SEARCH_DEPTH, true);
    }
    else {
        pv_ = minimax(board_s, INT_MIN, INT_MAX, SEARCH_DEPTH, false);
    }
    pv_.correctMoveSeq();
    return pv_;
};


PrincipalVariation MyEngine::minimax(const Board& board, int alpha, int beta, int depth, bool max) const {
    Move m;
    int val = board.eval();

    if (!depth)
        return PrincipalVariation(val);
    if(val > Piece::PIECE_VALUE[PieceType::King] / 2 || val < -Piece::PIECE_VALUE[PieceType::King] / 2) return PrincipalVariation(val);

    Board::MoveVec moves;
    Board board_s = board;
    board_s.pseudoLegalMoves(moves);
    PrincipalVariation best;

    if (max) {
        best = PrincipalVariation(INT_MIN);
        if (moves.empty()) { 
            return PrincipalVariation(0);
        }
        for (const auto& move : moves) {
            board_s = board;
            board_s.makeMove(move);
            PrincipalVariation res = minimax(board_s, alpha, beta, depth - 1, !max);
            if(res.getScore() > best.getScore()){
                best = res; 
                best.insertMove(move);
            }
            if (best.getScore() >= beta)
                break;
            alpha = std::max(alpha, best.getScore());
        }

    }
    else {
        best = PrincipalVariation(INT_MAX);
        if (moves.empty()) { 
            return PrincipalVariation(0);
        }
        for (const auto& move : moves) {
            board_s = board;
            board_s.makeMove(move);
            PrincipalVariation res = minimax(board_s, alpha, beta, depth - 1, !max);
            if(best.getScore() > res.getScore()){
                best = res;
                best.insertMove(move);
            }
            if (best.getScore() <= alpha)
                break;
            beta = std::min(beta, best.getScore());
        }
    }
    return best;
}