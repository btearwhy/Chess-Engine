#include "Piece.hpp"

#include <ostream>

Piece::Piece(){}

Piece::Piece(PieceColor color, PieceType type):pieceColor(color),pieceType(type){}

Piece::Piece(const Piece& piece):pieceColor(piece.color()), pieceType(piece.type()) {}

const int Piece::PIECE_VALUE[6] = { 100, 310, 325, 500, 900, 100000 };

Piece::Optional Piece::fromSymbol(char symbol) {
    switch (symbol)
    {
    case 'P': return { Piece(PieceColor::White, PieceType::Pawn) };
    case 'N': return { Piece(PieceColor::White, PieceType::Knight) };
    case 'B': return { Piece(PieceColor::White, PieceType::Bishop) };
    case 'R': return { Piece(PieceColor::White, PieceType::Rook) };
    case 'Q': return { Piece(PieceColor::White, PieceType::Queen) };
    case 'K': return { Piece(PieceColor::White, PieceType::King) };
    case 'p': return { Piece(PieceColor::Black, PieceType::Pawn) };
    case 'n': return { Piece(PieceColor::Black, PieceType::Knight) };
    case 'b': return { Piece(PieceColor::Black, PieceType::Bishop) };
    case 'r': return { Piece(PieceColor::Black, PieceType::Rook) };
    case 'q': return { Piece(PieceColor::Black, PieceType::Queen) };
    case 'k': return { Piece(PieceColor::Black, PieceType::King) };
    default:
        break;
    }
    return std::nullopt;
}

PieceColor Piece::color() const {
    return pieceColor;
}

PieceType Piece::type() const {
    return pieceType;
}

bool operator==(const Piece& lhs, const Piece& rhs) {
    return lhs.color() == rhs.color() && lhs.type() == rhs.type();
}

std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    PieceType p = piece.type();
    PieceColor c = piece.color();
    if (p == PieceType::Pawn && c == PieceColor::White) os << 'P';
    else if (p == PieceType::Knight && c == PieceColor::White) os << 'N';
    else if (p == PieceType::Bishop && c == PieceColor::White) os << 'B';
    else if (p == PieceType::Rook && c == PieceColor::White) os << 'R';
    else if (p == PieceType::Queen && c == PieceColor::White) os << 'Q';
    else if (p == PieceType::King && c == PieceColor::White) os << 'K';
    else if (p == PieceType::Pawn && c == PieceColor::Black) os << 'p';
    else if (p == PieceType::Knight && c == PieceColor::Black) os << 'n';
    else if (p == PieceType::Bishop && c == PieceColor::Black) os << 'b';
    else if (p == PieceType::Rook && c == PieceColor::Black) os << 'r';
    else if (p == PieceType::Queen && c == PieceColor::Black) os << 'q';
    else if (p == PieceType::King && c == PieceColor::Black) os << 'k';
    return os;
}

PieceColor operator!(PieceColor color) {
    return color == PieceColor::White ? PieceColor::Black:PieceColor::White;
}
