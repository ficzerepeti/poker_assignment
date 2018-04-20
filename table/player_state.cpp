#include "player_state.h"
#include "game_stages.h"

namespace poker_lib {

void print_actions(std::ostream& os, const std::vector<player_action_t>& container)
{
    if (container.empty())
    {
        os << "none";
    }
    for (const auto &elem : container)
    {
        std::visit([&os](const auto& action){ os << action; }, elem);
    }
}

std::ostream& operator<<(std::ostream &os, const per_game_player_state &state)
{
    return os << "has_folded: " << std::boolalpha << state.has_folded << std::noboolalpha
              << ", pocket_cards: " << state.pocket_cards.value_or("")
              << ", contribution_to_pot: " << state.contribution_to_pot;
}

std::ostream& operator<<(std::ostream &os, const per_betting_player_state &state)
{
    if (state.pre_flop_bets.empty()
        && state.flop_bets.empty()
        && state.turn_bets.empty()
        && state.river_bets.empty())
    {
        return os << "no actions";
    }

    if (!state.pre_flop_bets.empty())
    {
        os << "pre-flop actions: ";
        print_actions(os, state.pre_flop_bets);
    }
    if (!state.flop_bets.empty())
    {
        os << ", flop actions: ";
        print_actions(os, state.flop_bets);
    }
    if (!state.turn_bets.empty())
    {
        os << ", turn actions: ";
        print_actions(os, state.turn_bets);
    }
    if (!state.river_bets.empty())
    {
        os << ", river actions: ";
        print_actions(os, state.river_bets);
    }

    return os;
}

std::ostream& operator<<(std::ostream &os, const player_state &state)
{
    return os << "current_stack: " << state.current_stack
              << ", per_game_state: " << state.per_game_state
              << ", per_betting_state: " << state.per_betting_state;
}

bool operator==(const per_game_player_state &lhs, const per_game_player_state &rhs)
{
    return lhs.has_folded == rhs.has_folded &&
           lhs.pocket_cards == rhs.pocket_cards &&
           lhs.contribution_to_pot == rhs.contribution_to_pot;
}

bool operator==(const per_betting_player_state &lhs, const per_betting_player_state &rhs)
{
    return lhs.pre_flop_bets == rhs.pre_flop_bets
        && lhs.flop_bets == rhs.flop_bets
        && lhs.turn_bets == rhs.turn_bets
        && lhs.river_bets == rhs.river_bets;
}

bool operator==(const player_state &lhs, const player_state &rhs)
{
    return lhs.current_stack == rhs.current_stack &&
           lhs.per_game_state == rhs.per_game_state &&
           lhs.per_betting_state == rhs.per_betting_state;
}

std::vector<player_action_t> &player_state::get_actions(const game_stages stage)
{
    switch (stage)
    {
    case game_stages::deal_pocket_cards:
    case game_stages::pre_flop_betting_round:
        return per_betting_state.pre_flop_bets;

    case game_stages::deal_communal_cards:
    case game_stages::flop_betting_round:
        return per_betting_state.flop_bets;

    case game_stages::deal_turn_card:
    case game_stages::turn_betting_round:
        return per_betting_state.turn_bets;

    case game_stages::deal_river_card:
    case game_stages::river_betting_round:
    case game_stages::end_of_game:
        return per_betting_state.river_bets;
    }

    throw std::invalid_argument("Unknown stage enum value");
}

const std::vector<player_action_t> &player_state::get_actions(game_stages stage) const
{
    switch (stage)
    {
    case game_stages::deal_pocket_cards:
    case game_stages::pre_flop_betting_round:
        return per_betting_state.pre_flop_bets;

    case game_stages::deal_communal_cards:
    case game_stages::flop_betting_round:
        return per_betting_state.flop_bets;

    case game_stages::deal_turn_card:
    case game_stages::turn_betting_round:
        return per_betting_state.turn_bets;

    case game_stages::deal_river_card:
    case game_stages::river_betting_round:
    case game_stages::end_of_game:
        return per_betting_state.river_bets;
    }

    throw std::invalid_argument("Unknown stage enum value");
}

} // end of namespace poker_lib
