#ifndef CHESS_ENGINE_PIECE_HPP
#define CHESS_ENGINE_PIECE_HPP

#include <optional>
#include <iosfwd>

enum PieceColor {
    White,
    Black,
    Nb_color
};

enum PieceType {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    Nb_piece
};

class Piece {
public:

    using Optional = std::optional<Piece>;

    Piece();
    Piece(PieceColor color, PieceType type);
    Piece(const Piece& piece);
    static Optional fromSymbol(char symbol);

    PieceColor color() const;
    PieceType type() const;

    static const int PIECE_VALUE[PieceType::Nb_piece];
private:
    PieceColor pieceColor = PieceColor::White;
    PieceType pieceType = PieceType::Pawn; 
};

bool operator==(const Piece& lhs, const Piece& rhs);
std::ostream& operator<<(std::ostream& os, const Piece& piece);

// Invert a color (White becomes Black and vice versa)
PieceColor operator!(PieceColor color);

#endif
