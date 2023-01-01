#include "Board.hpp"

#include <ostream>
#include <cassert>
#include <cmath>

Board::Board()
{
}

Board::Board(const Board& board) {
    colorToPlay = board.colorToPlay;
    castling = board.castling;
    enPassant = board.enPassant;
    bitboards = board.bitboards;
    extraBitBoards = board.extraBitBoards;
}

Board Board::operator=(const Board& board){
    colorToPlay = board.colorToPlay;
    castling = board.castling;
    enPassant = board.enPassant;
    bitboards = board.bitboards;
    extraBitBoards = board.extraBitBoards;
    return *this;
}

void Board::setPiece(const Square& square, const Piece::Optional& piece) {
    std::optional<Piece> originalP = lookup(square.getBitPresent());
    if (originalP.has_value()) bitboards[originalP.value().color()][originalP.value().type()] ^= square.getBitPresent();
    bitboards[piece->color()][piece->type()] |= square.getBitPresent();
    updateExtraBitboard();
}

Piece::Optional Board::piece(const Square& square) const {
    for (int i = 0; i < PieceColor::Nb_color; i++) {
        for (int j = 0; j < PieceType::Nb_piece; j++) {
            if (bitboards[i][j] & square.getBitPresent()) {
                Piece p = Piece(PieceColor(i), PieceType(j));
                return { p };
            }
        }
    }
    return std::nullopt;
}

void Board::setTurn(PieceColor turn) {
    colorToPlay = turn;
}

PieceColor Board::turn() const {
    return colorToPlay;
}

void Board::setCastlingRights(CastlingRights cr) {
    castling = cr;
}

CastlingRights Board::castlingRights() const {
    return castling;
}

void Board::setEnPassantSquare(const Square::Optional& square) {
    enPassant = square;
}

Square::Optional Board::enPassantSquare() const {
    return enPassant;
}

std::optional<Piece> Board::lookup(U64 board) const {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            if (board & bitboards[i][j]) return Piece(PieceColor(i), PieceType(j));
        }
    }
    return std::nullopt;
}

void Board::updateExtraBitboard(){
    extraBitBoards[ExtraBoard::BlackPieces] = bitboards[PieceColor::Black][PieceType::Pawn] | bitboards[PieceColor::Black][PieceType::Knight] | bitboards[PieceColor::Black][PieceType::Bishop]
        | bitboards[PieceColor::Black][PieceType::Rook] | bitboards[PieceColor::Black][PieceType::Queen] | bitboards[PieceColor::Black][PieceType::King];
    extraBitBoards[ExtraBoard::WhitePieces] = bitboards[PieceColor::White][PieceType::Pawn] | bitboards[PieceColor::White][PieceType::Knight] | bitboards[PieceColor::White][PieceType::Bishop]
        | bitboards[PieceColor::White][PieceType::Rook] | bitboards[PieceColor::White][PieceType::Queen] | bitboards[PieceColor::White][PieceType::King];
    extraBitBoards[ExtraBoard::AllPieces] = extraBitBoards[ExtraBoard::BlackPieces] | extraBitBoards[ExtraBoard::WhitePieces];
}

void Board::makeMove(const Move& move) {
    PieceColor color = turn();
    U64 from = move.from().getBitPresent();
    U64 to = move.to().getBitPresent();
    int fromIndex = move.from().index();
    int toIndex = move.to().index();
    std::optional<Piece> fromP = lookup(from);
    std::optional<Piece> toP = lookup(to);
    bitboards[fromP.value().color()][fromP.value().type()] ^= from;
    bitboards[fromP.value().color()][fromP.value().type()] ^= to;
    if (toP != std::nullopt){
        bitboards[toP.value().color()][toP.value().type()] ^= to;
        if(toP.value().type() == PieceType::Rook){
            if(color == PieceColor::White){
                if(to & ROOK_SHORT_CASTLE_ORIGINAL[PieceColor::Black]){
                    castling &= ~CastlingRights::BlackKingside;
                }
                else if(to & ROOK_LONG_CASTLE_ORIGINAL[PieceColor::Black]){
                    castling &= ~CastlingRights::BlackQueenside;
                }
            }
            else{
                if(to & ROOK_SHORT_CASTLE_ORIGINAL[PieceColor::White]){
                    castling &= ~CastlingRights::WhiteKingside;
                }
                else if(to & ROOK_LONG_CASTLE_ORIGINAL[PieceColor::White]){
                    castling &= ~CastlingRights::WhiteQueenside;
                }
            }
        }
    }
    //promotion
    if (move.promotion() != std::nullopt) {
        bitboards[fromP.value().color()][move.promotion().value()] ^= to;
        bitboards[fromP.value().color()][fromP.value().type()] ^= to; //resume 0
    }
    //en passant
    if (fromP.value().type() == PieceType::Pawn && enPassant != std::nullopt && enPassant.value().getBitPresent() == to) {
        bitboards[reverseColor(color)][PieceType::Pawn] ^= backwardOne(color, to);
    }
    if (to == fowardOne(color, fowardOne(color, from)) && fromP.value().type() == PieceType::Pawn) {
        enPassant = Square(getIndex(backwardOne(color, to)));
    }
    else enPassant = std::nullopt;
    //castle
    if (fromP.value().type() == PieceType::King) {
        if(color == PieceColor::White) castling &= ~CastlingRights::White;
        else castling &= ~CastlingRights::Black;

        if (fromIndex == KING_ORIGINAL_INDEX[color] && toIndex == KING_ORIGINAL_INDEX[color] + 2) {
            bitboards[color][PieceType::Rook] ^= ROOK_SHORT_CASTLE_ORIGINAL[color];
            bitboards[color][PieceType::Rook] |= (ROOK_SHORT_CASTLE_ORIGINAL[color] << 2);
        }
        else if (fromIndex == KING_ORIGINAL_INDEX[color] && toIndex == KING_ORIGINAL_INDEX[color] - 2) {
            bitboards[color][PieceType::Rook] ^= ROOK_LONG_CASTLE_ORIGINAL[color];
            bitboards[color][PieceType::Rook] |= (ROOK_LONG_CASTLE_ORIGINAL[color] >> 3);
        }
    }

    if (fromP.value().type() == PieceType::Rook){
        if (from & ROOK_SHORT_CASTLE_ORIGINAL[color]) {
            if (color == PieceColor::White)
                castling &= ~CastlingRights::WhiteKingside;
            else
                castling &= ~CastlingRights::BlackKingside;
        }
        else if (from & ROOK_LONG_CASTLE_ORIGINAL[color]) {
            if (color == PieceColor::White)
                castling &= ~CastlingRights::WhiteQueenside;
            else
                castling &= ~CastlingRights::BlackQueenside;
        }
    }
    setTurn(reverseColor(color));
    updateExtraBitboard();
}

void Board::pseudoLegalMoves(MoveVec& moves) const{
    PieceColor color = turn();
    for (int i = 0; i < PieceType::Nb_piece; i++) {
        PieceType type = PieceType(i);
        U64 pieces = bitboards[color][type];
        while (pieces) {
            U64 piece = pieces & (~pieces + 1);
            U64 attackSet = getAttackSet(piece, color, type);
            Square from = Square(getIndex(piece));
            std::vector<Square> targets;
            boardsToSquares(targets, attackSet);
            for (Square target : targets) {
                if (checkPromotionPosition(color, target.index()) && PieceType::Pawn == type) {
                    for (int k = PieceType::Knight; k < PieceType::King; k++)
                        moves.emplace_back(Move(from, target, PieceType(k),  Piece::PIECE_VALUE[PieceType::Pawn]- Piece::PIECE_VALUE[k]));
                }
                else{
                    bool mark = true;
                    for(int k = 0;k <PieceType::Nb_piece; k++){
                        if(target.getBitPresent() & bitboards[reverseColor(color)][i]){
                            moves.emplace_back(Move(from, target, std::nullopt, -Piece::PIECE_VALUE[k]));
                            mark = false;
                        }
                    }
                    if(mark) moves.emplace_back(Move(from, target));
                }
            }
            pieces ^= piece;
        }
    }
    if (castling != CastlingRights::None) {
        U64 oppoAttackSet = getOpponentAttackSet(color);
        int fromIndex = KING_ORIGINAL_INDEX[color], toLongIndex = fromIndex - 2, toShortIndex = fromIndex + 2;
        CastlingRights kingside = CastlingRights::BlackKingside;
        CastlingRights queenside = CastlingRights::BlackQueenside;
        if (color == PieceColor::White) {
            kingside = CastlingRights::WhiteKingside;
            queenside = CastlingRights::WhiteQueenside;
        }

        if ((castling & kingside) != CastlingRights::None
            && !(kingCastleBetween(color) & extraBitBoards[ExtraBoard::AllPieces])
            && !(kingCastleMask(color) & oppoAttackSet)) {
            moves.emplace_back(Move(Square(fromIndex), Square(toShortIndex), std::nullopt, -100));
        }
        else if ((castling & queenside) != CastlingRights::None
            && !(queenCastleBetween(color) & extraBitBoards[ExtraBoard::AllPieces])
            && !(queenCastleMask(color) & oppoAttackSet)) {
            moves.emplace_back(Move(Square(fromIndex), Square(toLongIndex), std::nullopt, -100));
        }
    }
    std::sort(moves.begin(), moves.end(),[](Move a, Move b){
        return a.getRating() < b.getRating();
    });
}


U64 Board::getOpponentSet(PieceColor color) const {
    if (color == PieceColor::White) return extraBitBoards[PieceColor::Black];
    else return extraBitBoards[PieceColor::White];
}

U64 Board::getOwnSet(PieceColor color) const {
    if (color == PieceColor::White) return extraBitBoards[PieceColor::White];
    else return extraBitBoards[PieceColor::Black];
}

U64 Board::getOpponentAttackSet(PieceColor color) const {
    PieceColor revColor = reverseColor(color);
    return getPawnAttackSet(bitboards[revColor][PieceType::Pawn], revColor) | getKnightAttackSet(bitboards[revColor][PieceType::Knight], revColor)
        | getBishopAttackSet(bitboards[revColor][PieceType::Bishop], revColor) | getRookAttackSet(bitboards[revColor][PieceType::Rook], revColor)
        | getQueenAttackSet(bitboards[revColor][PieceType::Queen], revColor) | getKingAttackSetRegardless(bitboards[revColor][PieceType::King], revColor);

}


U64 Board::getAttackSet(U64 piece, PieceColor color, PieceType type) const {
    switch (type)
    {
    case PieceType::Pawn:
        return getPawnAttackSet(piece, color);
    case PieceType::Knight:
        return getKnightAttackSet(piece, color);
    case PieceType::Bishop:
        return getBishopAttackSet(piece, color);
    case PieceType::Rook:
        return getRookAttackSet(piece, color);
    case PieceType::Queen:
        return getQueenAttackSet(piece, color);
    case PieceType::King:
        return getKingAttackSet(piece, color);
    default:
        return 0ull;
    }
}

U64 Board::getPawnAttackSet(U64 pawn, PieceColor color) const{
    U64 fOne = fowardOne(color, pawn);
    U64 diagonalOne = westOne(fOne) | eastOne(fOne);
    U64 stepOneSet = fOne & ~extraBitBoards[ExtraBoard::AllPieces];
    U64 captureSet = diagonalOne & getOpponentSet(color);
    U64 stepTwoSet = fowardOne(color, fOne & ~extraBitBoards[ExtraBoard::AllPieces]) & ~extraBitBoards[ExtraBoard::AllPieces] & getRank(color, 4);
    U64 ep = empty;
    //enpassant
    if(enPassant != std::nullopt){
        ep |= (diagonalOne & enPassant.value().getBitPresent());
    }
    return stepOneSet | captureSet | stepTwoSet | ep;
}

U64 Board::getKnightAttackSet(U64 knight, PieceColor color) const {
    U64 step = nortOne(nortEastOne(knight)) | eastOne(nortEastOne(knight))
        | nortOne(nortWestOne(knight)) | westOne(nortWestOne(knight))
        | soutOne(soutEastOne(knight)) | eastOne(soutEastOne(knight))
        | soutOne(soutWestOne(knight)) | westOne(soutWestOne(knight));
    return step & ~getOwnSet(color);
}

U64 Board::getBishopAttackSet(U64 bishop, PieceColor color) const {
    U64 ne = bishop;
    U64 nw = bishop;
    U64 se = bishop;
    U64 sw = bishop;
    U64 attackSet = empty;
    while(ne) {
        ne = nortEastOne(ne);
        if(ne & getOpponentSet(color)) {
            attackSet |= (ne & getOpponentSet(color));
            ne &= ~getOpponentSet(color);
        }
        if (ne & getOwnSet(color)) {
            ne &= ~getOwnSet(color);
        }
        attackSet |= ne;
    }

    while (nw) {
        nw = nortWestOne(nw);
        if (nw & getOpponentSet(color)) {
            attackSet |= (nw & getOpponentSet(color));
            nw &= ~getOpponentSet(color);
        }
        if (nw & getOwnSet(color)) {
            nw &= ~getOwnSet(color);
        }
        attackSet |= nw;
    }

    while (se) {
        se = soutEastOne(se);
        if (se & getOpponentSet(color)) {
            attackSet |= (se & getOpponentSet(color));
            se &= ~getOpponentSet(color);
        }
        if (se & getOwnSet(color)) {
            se &= ~getOwnSet(color);
        }
        attackSet |= se;
    }

    while (sw) {
        sw = soutWestOne(sw);
        if (sw & getOpponentSet(color)) {
            attackSet |= (sw & getOpponentSet(color));
            sw &= ~getOpponentSet(color);
        }
        if (sw & getOwnSet(color)) {
            sw &= ~getOwnSet(color);
        }
        attackSet |= sw;
    }
    return attackSet;
}

U64 Board::getRookAttackSet(U64 rook, PieceColor color) const {
    U64 n = rook;
    U64 w = rook;
    U64 e = rook;
    U64 s = rook;
    U64 attackSet = empty;
    while (n) {
        n = nortOne(n);
        if (n & getOpponentSet(color)) {
            attackSet |= (n & getOpponentSet(color));
            n &= ~getOpponentSet(color);
        }
        if (n & getOwnSet(color)) {
            n &= ~getOwnSet(color);
        }
        attackSet |= n;
    }

    while (w) {
        w = westOne(w);
        if (w & getOpponentSet(color)) {
            attackSet |= (w & getOpponentSet(color));
            w &= ~getOpponentSet(color);
        }
        if (w & getOwnSet(color)) {
            w &= ~getOwnSet(color);
        }
        attackSet |= w;
    }

    while (e) {
        e = eastOne(e);
        if (e & getOpponentSet(color)) {
            attackSet |= (e & getOpponentSet(color));
            e &= ~getOpponentSet(color);
        }
        if (e & getOwnSet(color)) {
            e &= ~getOwnSet(color);
        }
        attackSet |= e;
    }

    while (s) {
        s = soutOne(s);
        if (s & getOpponentSet(color)) {
            attackSet |= (s & getOpponentSet(color));
            s &= ~getOpponentSet(color);
        }
        if (s & getOwnSet(color)) {
            s &= ~getOwnSet(color);
        }
        attackSet |= s;
    }
    return attackSet;
}

U64 Board::getQueenAttackSet(U64 queen, PieceColor color) const {
    return getRookAttackSet(queen, color) | getBishopAttackSet(queen, color);
}

U64 Board::getKingAttackSet(U64 king, PieceColor color) const {
    return (nortOne(king) | nortEastOne(king) | eastOne(king) | soutEastOne(king) | soutOne(king) | soutWestOne(king) | westOne(king) | nortWestOne(king)) & ~getOwnSet(color) & ~getOpponentAttackSet(color);
}

U64 Board::getKingAttackSetRegardless(U64 king, PieceColor color) const {
    return (nortOne(king) | nortEastOne(king) | eastOne(king) | soutEastOne(king) | soutOne(king) | soutWestOne(king) | westOne(king) | nortWestOne(king)) & ~getOwnSet(color);
}
void Board::boardsToSquares(std::vector<Square>& squares, U64 board) const{
    while (board) {
        U64 piece = board & (~board + 1);
        int id = getIndex(piece);
        squares.emplace_back(Square(id));
        

        board ^= piece;
    }
}

int Board::eval() const{
    int materia = evalMaterialValue(PieceColor::White) - evalMaterialValue(PieceColor::Black);
    return materia;
}


int Board::evalMaterialValue(PieceColor color) const {
    int score = 0;
    for (int i = 0; i < PieceType::Nb_piece; i++) {
        score += Piece::PIECE_VALUE[i] * popCount(bitboards[color][i]);
    }
    return score;
}



void Board::pseudoLegalMovesFrom(const Square& from,
                                 Board::MoveVec& moves) const {
    PieceColor color = turn();
    auto f = lookup(from.getBitPresent());
    if (f == std::nullopt) return;
    Piece piece = lookup(from.getBitPresent()).value();
    U64 attackSet = getAttackSet(from.getBitPresent(), turn(), piece.type());
    std::vector<Square> targets;
    boardsToSquares(targets, attackSet);
    for (Square target : targets) {
        if (checkPromotionPosition(color, target.index()) && PieceType::Pawn == piece.type()) {
            for (int k = PieceType::Knight; k < PieceType::King; k++)
                moves.emplace_back(Move(from, target, PieceType(k)));
        }
        else moves.emplace_back(Move(from, target, std::nullopt));
    }

        if (castling != CastlingRights::None) {
        U64 oppoAttackSet = getOpponentAttackSet(color);
        int fromIndex = KING_ORIGINAL_INDEX[color], toLongIndex = fromIndex - 2, toShortIndex = fromIndex + 2;
        CastlingRights kingside = CastlingRights::BlackKingside;
        CastlingRights queenside = CastlingRights::BlackQueenside;
        if (color == PieceColor::White) {
            kingside = CastlingRights::WhiteKingside;
            queenside = CastlingRights::WhiteQueenside;
        }

        if ((castling & kingside) != CastlingRights::None
            && !(kingCastleBetween(color) & extraBitBoards[ExtraBoard::AllPieces])
            && !(kingCastleMask(color) & oppoAttackSet)) {
            moves.emplace_back(Move(Square(fromIndex), Square(toShortIndex)));
        }
        else if ((castling & queenside) != CastlingRights::None
            && !(queenCastleBetween(color) & extraBitBoards[ExtraBoard::AllPieces])
            && !(queenCastleMask(color) & oppoAttackSet)) {
            moves.emplace_back(Move(Square(fromIndex), Square(toLongIndex)));
        }
    }
     std::sort(moves.begin(), moves.end(),[](Move a, Move b){
        return a.getRating() < b.getRating();
    });

}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    U64 mask = 0x8000000000000000;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::optional<Piece> p = board.lookup(mask);
            mask = mask >> 1;
            if (p.has_value()) {
                os << p.value();
            }
            else os << '.';
            os << ' ';
        }
        os << std::endl;
    }
    return os;
}
