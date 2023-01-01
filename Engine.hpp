#ifndef CHESS_ENGINE_ENGINE_HPP
#define CHESS_ENGINE_ENGINE_HPP

#include "PrincipalVariation.hpp"
#include "Board.hpp"
#include "TimeInfo.hpp"

#include <string>
#include <optional>
#include <cstddef>
#include <climits>


struct HashInfo {
    std::size_t defaultSize;
    std::size_t minSize;
    std::size_t maxSize;
};

class Engine {
public:

    virtual ~Engine() = default;

    virtual std::string name() const = 0;
    virtual std::string version() const = 0;
    virtual std::string author() const = 0;

    virtual void newGame() = 0;
    virtual PrincipalVariation pv(
        const Board& board,
        const TimeInfo::Optional& timeInfo = std::nullopt
    ) = 0;

    virtual std::optional<HashInfo> hashInfo() const;
    virtual void setHashSize(std::size_t size);
};


class MyEngine :public Engine {
public:
    std::optional<HashInfo> hashInfo() const { return std::nullopt; }
    void setHashSize(std::size_t size) { (void)size; }
    std::string name() const{
        return "Morph";
    }
       
    std::string version() const {
        return "1.0";
    }

    std::string author() const{
        return "Mofan Deng";
    }

    const static int SEARCH_DEPTH;


    void newGame();
    PrincipalVariation pv(
        const Board& board,
        const TimeInfo::Optional& timeInfo
    );
    PrincipalVariation minimax(const Board& boardOriginal, int alpha, int beta, int depth, bool max) const;
};

#endif
