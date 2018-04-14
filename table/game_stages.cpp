#include <type_traits>
#include "game_stages.h"

#define HANDLE_GAME_STAGE_OSTREAM(name) case poker_lib::game_stages::name: return os << #name;

namespace poker_lib {

std::ostream &operator<<(std::ostream &os, const game_stages stage)
{
    switch (stage)
    {
    HANDLE_GAME_STAGE_OSTREAM(deal_pocket_cards);
    HANDLE_GAME_STAGE_OSTREAM(pre_flop_betting_round);
    HANDLE_GAME_STAGE_OSTREAM(deal_communal_cards);
    HANDLE_GAME_STAGE_OSTREAM(flop_betting_round);
    HANDLE_GAME_STAGE_OSTREAM(deal_turn_card);
    HANDLE_GAME_STAGE_OSTREAM(turn_betting_round);
    HANDLE_GAME_STAGE_OSTREAM(deal_river_card);
    HANDLE_GAME_STAGE_OSTREAM(river_betting_round);
    HANDLE_GAME_STAGE_OSTREAM(end_of_game);
    }

    return os << "unknown";
}

game_stages get_next_game_stage(const game_stages stage)
{
    using game_stages_under_t = std::underlying_type<game_stages>::type;

    const auto stage_val = static_cast<game_stages_under_t>(stage);
    const bool is_okay_to_increment = stage_val < static_cast<game_stages_under_t>(game_stages::end_of_game);

    return static_cast<game_stages>(is_okay_to_increment ? (stage_val + 1) : stage_val);
}

} // end of namespace poker_lib