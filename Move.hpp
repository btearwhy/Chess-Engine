#ifndef CHESS_ENGINE_MOVE_HPP
#define CHESS_ENGINE_MOVE_HPP

#include "Square.hpp"
#include "Piece.hpp"
#include "CastlingRights.hpp"

#include <iosfwd>
#include <optional>
#include <string>

class Move {
public:

    using Optional = std::optional<Move>;
    Move();
    Move(const Move& move);
    Move(const Square& from, const Square& to,
        const std::optional<PieceType>& promotion = std::nullopt, int rating = 0);
    static Optional fromUci(const std::string& uci);

    Square from() const;
    Square to() const;
    std::optional<PieceType> promotion() const;
    int getRating() const;
    Move operator=(const Move &m);
private:
    Square fromP;
    Square toP;
    std::optional<PieceType> promo = std::nullopt;
    int rating;
};

std::ostream& operator<<(std::ostream& os, const Move& move);

// Needed for std::map, std::set
bool operator<(const Move& lhs, const Move& rhs);
bool operator==(const Move& lhs, const Move& rhs);


#endif
