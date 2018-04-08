#include "player_actions.h"

namespace poker_lib {

std::ostream& operator<<(std::ostream &os, const player_action_fold &obj)
{
    return os << "fold";
}

std::ostream& operator<<(std::ostream &os, const player_action_check_or_call &obj)
{
    return os << "check or call";
}

std::ostream& operator<<(std::ostream &os, const player_action_raise &obj)
{
    return os << "raise with amount " << obj.amount_raised_above_call;
}

} // end of namespace poker_lib
