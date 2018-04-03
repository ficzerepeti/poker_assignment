#pragma once

#include <string>

namespace poker_lib {

class i_deck_interaction
{
public:
    virtual ~i_deck_interaction() = default;

    virtual std::string get_pocket_cards() = 0;
    virtual std::string get_flop() = 0;
    virtual std::string get_turn() = 0;
    virtual std::string get_river() = 0;
};

} // end of namespace poker_lib
