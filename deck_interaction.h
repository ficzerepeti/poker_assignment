#pragma once

#include "i_deck_interaction.h"

namespace poker_lib {

class deck_interaction : public i_deck_interaction
{
public:
    ~deck_interaction() override = default;

    std::string get_pocket_cards() override;
    std::string get_flop() override;
    std::string get_turn() override;
    std::string get_river() override;

};

} // end of namespace poker_lib
