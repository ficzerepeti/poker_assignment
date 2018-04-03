#pragma once

#include <cstdint>
#include <variant>

namespace poker_lib {

struct user_action_fold{};
struct user_action_check{};
struct user_action_call
{
    uint64_t amount = 0;
};
struct user_action_raise
{
    uint64_t amount = 0;
};

class i_user_interaction
{
public:
    using user_action_t = std::variant<user_action_fold, user_action_check, user_action_call, user_action_raise>;

    virtual ~i_user_interaction() = default;

    virtual user_action_t get_user_action(size_t position, const user_action_t& recommended_action) = 0;
    virtual user_action_t get_opponent_action(size_t position) = 0;
};

} // end of namespace poker_lib
