#include "Move.hpp"

#include <ostream>


Move::Move() {}

Move::Move(const Move& move) : fromP(move.from()), toP(move.to()), promo(move.promotion()), rating(move.rating) {}

Move::Move(const Square& from, const Square& to,
           const std::optional<PieceType>& promotion, int rating):fromP(from), toP(to), promo(promotion), rating(rating) {}


Move::Optional Move::fromUci(const std::string& uci) {
    if (uci.size() > 5 || uci.size() < 4 || uci[0] < 'a' || uci[0] > 'h' || uci[1] < '1' || uci[1] > '8' || uci[2] < 'a' || uci[2] > 'h' || uci[3] < '1' || uci[3] > '8') return std::nullopt;
    Square from = Square::fromName(uci.substr(0, 2)).value();
    Square to = Square::fromName(uci.substr(2, 2)).value();    
    std::optional<PieceType> promo = std::nullopt;
    if (uci.length() == 5) {
        if (uci[4] == 'n') promo = PieceType::Knight;
        else if (uci[4] == 'r') promo = PieceType::Rook;
        else if (uci[4] == 'b') promo = PieceType::Bishop;
        else if (uci[4] == 'q') promo = PieceType::Queen;
        else return std::nullopt;
 
    }
    return { Move(from, to, promo) };
}

Square Move::from() const {
    return fromP;
}

Square Move::to() const {
    return toP;
}

std::optional<PieceType> Move::promotion() const {
    return promo;
}

int Move::getRating() const{
    return rating;
}

Move Move::operator=(const Move &m){
    this->fromP = m.from();
    this->toP = m.to();
    this->promo = m.promotion();
    this->rating = m.rating;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << move.from() << move.to();
    if (move.promotion() != std::nullopt) {
        switch (move.promotion().value())
        {
        case PieceType::Knight: os << 'n'; break;
        case PieceType::Bishop: os << 'b'; break;
        case PieceType::Rook: os << 'r'; break;
        case PieceType::Queen: os << 'q'; break;
        default:
            break;
        }
    }
    return os;
}


bool operator<(const Move& lhs, const Move& rhs) {
    if (lhs.from().index() != rhs.from().index())
        return lhs.from().index() < rhs.from().index();

    if (lhs.to().index() != rhs.to().index())
        return lhs.to().index() < rhs.to().index();

    if (lhs.promotion().has_value() && rhs.promotion().has_value())
        return (lhs.promotion().value() < rhs.promotion().value());

    if (!lhs.promotion().has_value() && rhs.promotion().has_value())
        return true;

    if (lhs.promotion().has_value() && !rhs.promotion().has_value())
        return false;
    return false;
}

bool operator==(const Move& lhs, const Move& rhs) {
    if (lhs.promotion().has_value() && rhs.promotion().has_value())
        return (lhs.from() == rhs.from() && lhs.to() == rhs.to() && lhs.promotion().value() == rhs.promotion().value());

    if (lhs.promotion().has_value() && !rhs.promotion().has_value())
        return false;

    if (!lhs.promotion().has_value() && rhs.promotion().has_value())
        return false;

    return (lhs.from() == rhs.from() && lhs.to() == rhs.to());
}
