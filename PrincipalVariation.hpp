#ifndef CHESS_ENGINE_PRINCIPALVARIATION_HPP
#define CHESS_ENGINE_PRINCIPALVARIATION_HPP

#include "Move.hpp"
#include "Piece.hpp"

#include <iosfwd>
#include <cstddef>
#include <vector>

class PrincipalVariation {
public:
    PrincipalVariation();
    PrincipalVariation(int s);
    PrincipalVariation(const PrincipalVariation& pv);
    PrincipalVariation(std::vector<Move> moves);
    PrincipalVariation operator=(const PrincipalVariation& pv);
    using MoveIter = std::vector<Move>::const_iterator;

    bool isMate() const;
    int score() const;
    void correctMoveSeq();
    std::size_t length() const;
    MoveIter begin() const;
    MoveIter end() const;

    int getScore() const;

    void setScore(int score_);
    void insertMove(Move move);
    std::vector<Move> getMoves();
private:
    std::vector<Move> plies;
    int score_;

};

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv);

#endif
