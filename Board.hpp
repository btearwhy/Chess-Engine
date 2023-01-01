 #ifndef CHESS_ENGINE_BOARD_HPP
#define CHESS_ENGINE_BOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"
#include "PrincipalVariation.hpp"

#include <optional>
#include <iosfwd>
#include <vector>
#include <array>
#include <algorithm>

#ifndef U64
#define U64 std::uint64_t
#endif 

enum ExtraBoard {
    WhitePieces,
    BlackPieces,
    AllPieces,
    Nb_extraBoards
};

const U64 empty = 0x0000000000000000;
const U64 universe = 0xFFFFFFFFFFFFFFFF;

const U64 fileA = 0x8080808080808080;
const U64 fileB = 0x4040404040404040;
const U64 fileC = 0x2020202020202020;
const U64 fileD = 0x1010101010101010;
const U64 fileE = 0x0808080808080808;
const U64 fileF = 0x0404040404040404;
const U64 fileG = 0x0202020202020202;
const U64 fileH = 0x0101010101010101;

const U64 rank[8] = { 0x00000000000000FF,
                      0x000000000000FF00,
                      0x0000000000FF0000, 
                      0x00000000FF000000, 
                      0x000000FF00000000, 
                      0x0000FF0000000000,
                      0x00FF000000000000, 
                      0xFF00000000000000 };

const int KING_ORIGINAL_INDEX[2] = { 4, 60 };
const U64 ROOK_SHORT_CASTLE_ORIGINAL[2] = { 0x0000000000000001, 0x0100000000000000 };
const U64 ROOK_LONG_CASTLE_ORIGINAL[2] = { 0x0000000000000080, 0x8000000000000000 };
 


class Board {
public:

    using Optional = std::optional<Board>;
    using MoveVec = std::vector<Move>;

    Board();
    Board(const Board& board);
    Board operator=(const Board& board);
    void setPiece(const Square& square, const Piece::Optional& piece);
    Piece::Optional piece(const Square& square) const;
    void setTurn(PieceColor turn);
    PieceColor turn() const;
    void setCastlingRights(CastlingRights cr);
    CastlingRights castlingRights() const;
    void setEnPassantSquare(const Square::Optional& square);
    Square::Optional enPassantSquare() const;
    void updateExtraBitboard();

    PrincipalVariation getPV();
    std::optional<Piece> lookup(U64 board) const;

    U64 getAttackSet(U64 piece, PieceColor color, PieceType type) const;
    U64 getPawnAttackSet(U64 pawn, PieceColor color) const;
    U64 getKnightAttackSet(U64 knight, PieceColor color) const;
    U64 getBishopAttackSet(U64 bishop, PieceColor color) const;
    U64 getRookAttackSet(U64 rook, PieceColor color) const;
    U64 getQueenAttackSet(U64 queen, PieceColor color) const;
    U64 getKingAttackSet(U64 king, PieceColor color) const;
    U64 getKingAttackSetRegardless(U64 king, PieceColor color) const;
    U64 getOpponentSet(PieceColor color) const;
    U64 getOwnSet(PieceColor color) const;
    
    U64 getOpponentAttackSet(PieceColor color) const;
    void boardsToSquares(std::vector<Square>& squares, U64 board) const;

    int eval() const;
    int evalMaterialValue(PieceColor color) const;

    void makeMove(const Move& move);


    void pseudoLegalMoves(MoveVec& moves) const;
    void pseudoLegalMovesFrom(const Square& from, MoveVec& moves) const;

    U64 soutOne(U64 b) const{ return  (b >> 8) & ~rank[7]; }
    U64 nortOne(U64 b) const{ return  (b << 8) & ~rank[0]; }
    U64 eastOne(U64 b) const{ return (b >> 1) & ~fileA; }
    U64 westOne(U64 b) const{ return (b << 1) & ~fileH; }
    U64 nortEastOne(U64 b) const{ return (b << 7) & ~fileA & ~rank[0]; }
    U64 nortWestOne(U64 b) const{ return (b << 9) & ~fileH & ~rank[0]; }
    U64 soutEastOne(U64 b) const{ return (b >> 9) & ~fileA & ~rank[7]; }
    U64 soutWestOne(U64 b) const{ return (b >> 7) & ~fileH & ~rank[7]; }

    U64 fowardOne(PieceColor color, U64 b) const{
        if (color == PieceColor::White) return nortOne(b);
        else return soutOne(b);
    }

    U64 backwardOne(PieceColor color, U64 b) const{
        if (color == PieceColor::White) return soutOne(b);
        else return nortOne(b);
    }
    U64 nearRank(PieceColor color) const{
        if (color == PieceColor::White) return rank[7];
        else return rank[0];
    }
    U64 farRank(PieceColor color) const{
        if (color == PieceColor::White) return rank[0];
        else return rank[7];
    }

    U64 getRank(PieceColor color, int r) const{
        if (color == PieceColor::White) return rank[r - 1];
        else return rank[8 - r];
    }

    int getIndex(U64 board) const{
        int cnt = -1;
        while (board) {
            cnt++;
            board = board >> 1;
        }
        return (cnt / 8) * 8 + 7 - cnt % 8;
    }

    bool checkPromotionPosition(PieceColor color, int index) const{
        if (color == PieceColor::White) return index >= 56 && index <= 63;
        else return index >= 0 && index <= 7;
    }

    bool isBorder(U64 piece) const{
        return piece & (fileA | fileH | rank[0] | rank[7]);
    }

    U64 kingCastleBetween(PieceColor color) const{
        if (color == PieceColor::White) return 0x0000000000000006;
        else return 0x0600000000000000;
    }

    U64 queenCastleBetween(PieceColor color) const{
        if (color == PieceColor::White) return 0x0000000000000070;
        else return 0x7000000000000000;
    }

    U64 kingCastleMask(PieceColor color) const{
        if (color == PieceColor::White) return 0x000000000000000E;
        else return 0x0E00000000000000;
    }

    U64 queenCastleMask(PieceColor color) const{
        if (color == PieceColor::White) return 0x0000000000000038;
        else return 0x3800000000000000;
    }

    PieceColor reverseColor(PieceColor color) const{
        if (color == PieceColor::White) return PieceColor::Black;
        return PieceColor::White;
    }

    int popCount(U64 x) const{
        int count = 0;
        while (x) {
            count++;
            x &= x - 1; 
        }
        return count;
    }


private:

    std::array<std::array<U64, 6>, 2> bitboards = {};

    std::array<U64, ExtraBoard::Nb_extraBoards> extraBitBoards = {0};

    PieceColor colorToPlay = PieceColor::White;
    CastlingRights castling;
    std::optional<Square> enPassant;


};

std::ostream& operator<<(std::ostream& os, const Board& board);





#endif
