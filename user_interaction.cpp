#include "user_interaction.h"

namespace poker_lib {

user_interaction::user_action_t user_interaction::get_user_action(size_t position,
                                                const user_action_t &recommended_action)
{
    return user_action_raise{50000};
}

user_interaction::user_action_t user_interaction::get_opponent_action(size_t position)
{
    return user_action_fold{};
}

} // end of namespace poker_lib
