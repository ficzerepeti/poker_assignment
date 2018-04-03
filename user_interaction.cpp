#include "user_interaction.h"

namespace poker_lib {

std::string user_interaction::get_pocket_cards()
{
    return "Ac Ad";
}

std::string user_interaction::get_flop()
{
    return "Ah As 3c";
}

std::string user_interaction::get_turn()
{
    return "4d";
}

std::string user_interaction::get_river()
{
    return "5h";
}

user_action_t user_interaction::get_user_action(size_t position,
                                                const user_action_t &recommended_action)
{
    return user_action_raise{50000};
}

user_action_t user_interaction::get_opponent_action(size_t position)
{
    return user_action_fold{};
}

} // end of namespace poker_lib
