#include "PrincipalVariation.hpp"

#include <ostream>
#include <algorithm>

PrincipalVariation::PrincipalVariation(){}

PrincipalVariation::PrincipalVariation(int s):score_(s){}

PrincipalVariation::PrincipalVariation(const PrincipalVariation& pv){
    score_ = pv.score_;
    plies = pv.plies;
}

PrincipalVariation PrincipalVariation::operator=(const PrincipalVariation& pv){
    score_ = pv.score_;
    plies = pv.plies;
    return *this;
}

PrincipalVariation::PrincipalVariation(std::vector<Move> moves) :plies(moves){}

bool PrincipalVariation::isMate() const {
    return score_ > Piece::PIECE_VALUE[PieceType::King] / 2 || score_ < -Piece::PIECE_VALUE[PieceType::King] / 2;
}

int PrincipalVariation::score() const {
    if (isMate()) {
        return (int)plies.size() - 2;
    }
    else {
        return score_;
    }
}

int PrincipalVariation::getScore() const{
    return score_;
}

std::vector<Move> PrincipalVariation::getMoves(){
    return plies;
}

void PrincipalVariation::correctMoveSeq(){
    std::reverse(plies.begin(), plies.end());
}

std::size_t PrincipalVariation::length() const {
    if(isMate())
        return plies.size() - 2;
    else return plies.size();
}

PrincipalVariation::MoveIter PrincipalVariation::begin() const {
    return plies.begin();
}

PrincipalVariation::MoveIter PrincipalVariation::end() const {
    return plies.end();
}

void PrincipalVariation::insertMove(Move move) {
    plies.emplace_back(move);
}


void PrincipalVariation::setScore(int score_t) {
    score_ = score_t;
}

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv) {
    for (Move a : pv) {
        os << a << " ";
    }
    return os;
}

