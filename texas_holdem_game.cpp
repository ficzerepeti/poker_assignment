#include <stdexcept>
#include <sstream>
#include "texas_holdem_game.h"

// Helper to be able to visit with lambdas without if constexpr.
template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// Type deduction guide
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace poker_lib {

texas_holdem_game::texas_holdem_game(i_user_interaction &user_interaction,
                                     i_deck_interaction& deck_interaction,
                                     const size_t user_stack,
                                     const size_t big_blind_size,
                                     const size_t num_of_players,
                                     const size_t user_pos)
:
    _user_interaction(user_interaction),
    _deck_interaction(deck_interaction),
    _big_blind_size(big_blind_size),
    _players(num_of_players)
{
    if (_players.size() < 2)
    {
        throw std::invalid_argument("Cannot play poker with less than two players at the table");
    }
    if (_players.size() <= user_pos)
    {
        std::ostringstream oss;
        oss << "User position is set as " << user_pos << " which is not possible at a table size of " << _players.size();
        throw std::invalid_argument(oss.str());
    }
    _players[user_pos].is_our_user = true;
    _players[user_pos].user_stack = user_stack;
}

void texas_holdem_game::run_game()
{
    // Pre-flop
    _pot_size = _big_blind_size + _big_blind_size / 2;

    _pocket_cards = _deck_interaction.get_pocket_cards();
    run_betting_round();

    // Flop
    _board = _deck_interaction.get_flop();
    run_betting_round();

    // Turn
    _board += _deck_interaction.get_turn();
    run_betting_round();

    // Turn
    _board += _deck_interaction.get_river();
    run_betting_round();

    execute_showdown();

}

bool texas_holdem_game::run_betting_round()
{
    auto num_of_active_players = _players.size();
    for (const auto& player : _players)
    {
        if (player.has_folded())
        {
            --num_of_active_players;
        }
    }
    auto checks_or_folds_needed = num_of_active_players;

    for (auto curr_player_pos = 2 % _players.size();
         num_of_active_players > 1 && checks_or_folds_needed > 0;
         curr_player_pos = (curr_player_pos + 1) % _players.size())
    {
        auto& player = _players.at(curr_player_pos);
        if (player.has_folded())
        {
            continue;
        }

        if (player.is_our_user)
        {
            const auto recommended_action = user_action_check{}; // TODO
            player.actions_taken.emplace_back(_user_interaction.get_user_action(curr_player_pos, recommended_action));
        }
        else
        {
            player.actions_taken.emplace_back(_user_interaction.get_opponent_action(curr_player_pos));
        }

        std::visit(overloaded {
            [&](user_action_fold arg) {
                if (checks_or_folds_needed > 0) { --checks_or_folds_needed; }
                --num_of_active_players;
            },
            [&](user_action_check arg) {
                if (checks_or_folds_needed > 0) { --checks_or_folds_needed; }
            },
            [&](const user_action_call &arg) {
                if (checks_or_folds_needed > 0) { --checks_or_folds_needed; }
                _pot_size += arg.amount;
            },
            [&](const user_action_raise& arg) {
                _pot_size += arg.amount;
                checks_or_folds_needed = num_of_active_players - 1;
            }
        }, player.actions_taken.back());
    }

    return false;
}

} // end of namespace poker_lib