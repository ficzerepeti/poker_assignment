#include "player_actions.h"

namespace poker_lib {

std::ostream& operator<<(std::ostream &os, const player_action_fold &obj)
{
    return os << "fold";
}

std::ostream& operator<<(std::ostream &os, const player_action_check &obj)
{
    return os << "check";
}

std::ostream& operator<<(std::ostream &os, const player_action_call &obj)
{
    return os << "call with amount " << obj.amount;
}

std::ostream& operator<<(std::ostream &os, const player_action_raise &obj)
{
    return os << "raise with amount " << obj.amount;
}

} // end of namespace poker_lib
