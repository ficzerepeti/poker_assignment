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

    std::string get_pocket_cards() override;
    std::string get_flop() override;
    std::string get_turn() override;
    std::string get_river() override;

    player_action_t get_user_action(const player_action_t &recommended_action) override;
    player_action_t get_opponent_action() override;

    void notify_player(const std::string &message) override;

private:
    player_action_t read_player_action();

    std::ostream& _os;
    std::istream& _is;
};

} // end of namespace poker_lib
