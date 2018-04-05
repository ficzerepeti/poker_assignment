#pragma once

#include <vector>
#include "i_analysis_engine.h"
#include "i_my_poker_lib.h"

namespace poker_lib {

class analysis_engine : public i_analysis_engine
{
public:
    analysis_engine(i_my_poker_lib &poker_lib, size_t num_of_players);
    ~analysis_engine() override = default;

    bool set_board(const std::string &cards) override;
    bool set_player_pocket_cards(size_t distance_from_dealer, const std::string &cards) override;
    void set_player_folded(size_t distance_from_dealer) override;

    user_analysis make_user_analysis(size_t distance_from_dealer) override;

private:
    struct player_state
    {
        std::string pocket_cards = "random";
        bool has_folded = false;
    };

    i_my_poker_lib& _poker_lib;
    std::string _board;
    std::vector<player_state> _players;
};

} // end of namespace poker_lib
