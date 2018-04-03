#pragma once

#include <istream>
#include <ostream>

#include "i_user_interaction.h"

namespace poker_lib {

class streamed_user_interaction : public i_user_interaction
{
public:
    streamed_user_interaction(std::ostream& os, std::istream& is);
    ~streamed_user_interaction() override = default;

    player_action_t get_user_action(size_t position, const player_action_t &recommended_action) override;
    player_action_t get_opponent_action(size_t position) override;

    void notify_player(const std::string &message) override;

private:
    player_action_t read_player_action();
    uint64_t read_amount();

    std::ostream& _os;
    std::istream& _is;
};

} // end of namespace poker_lib
