#include "deck_interaction.h"

namespace poker_lib {

std::string deck_interaction::get_pocket_cards()
{
    return "Ac Ad";
}

std::string deck_interaction::get_flop()
{
    return "Ah As 3c";
}

std::string deck_interaction::get_turn()
{
    return "4d";
}

std::string deck_interaction::get_river()
{
    return "5h";
}

} // end of namespace poker_lib