#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <type_traits>

#include "texas_holdem_game.h"

template<class T> struct always_false : std::false_type {};

namespace poker_lib {

texas_holdem_game::texas_holdem_game(i_user_interaction& user_interaction,
                                     const size_t user_stack,
                                     const size_t big_blind_size,
                                     const size_t num_of_players,
                                     const size_t user_pos)
:
    _user_interaction(user_interaction),
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
    _players[user_pos].stack = user_stack;
}

void texas_holdem_game::run_game()
{
    // Pre-flop
    _players.at(0).investment_in_pot = _big_blind_size / 2;
    _players.at(1).investment_in_pot = _big_blind_size;
    _pot_size = _big_blind_size + _big_blind_size / 2;
    _max_investment = _big_blind_size;

    _pocket_cards = _user_interaction.get_pocket_cards();
    {
        std::ostringstream oss;
        oss << "Pre-flop: Pot size is " << _pot_size << ", pocket cards: " << _pocket_cards;
        _user_interaction.notify_player(oss.str());
    }
    bool game_has_ended = run_betting_round();

    // Flop
    if (!game_has_ended)
    {
        _board = _user_interaction.get_flop();
        {
            std::ostringstream oss;
            oss << "Flop: Pot size is " << _pot_size << ", pocket cards: " << _pocket_cards << ", board: " << _board;
            _user_interaction.notify_player(oss.str());
        }
        game_has_ended = run_betting_round();
    }

    // Turn
    if (!game_has_ended)
    {
        _board += _user_interaction.get_turn();
        {
            std::ostringstream oss;
            oss << "Turn: Pot size is " << _pot_size << ", pocket cards: " << _pocket_cards << ", board: " << _board;
            _user_interaction.notify_player(oss.str());
        }
        game_has_ended = run_betting_round();
    }

    // River
    if (!game_has_ended)
    {
        _board += _user_interaction.get_river();
        {
            std::ostringstream oss;
            oss << "River: Pot size is " << _pot_size << ", pocket cards: " << _pocket_cards << ", board: " << _board;
            _user_interaction.notify_player(oss.str());
        }
        game_has_ended = run_betting_round();
    }

    if (game_has_ended)
    {
        const auto iter = std::find_if(_players.begin(), _players.end(), [](const player_state& state){ return !state.has_folded(); });
        const auto winner_pos = static_cast<size_t>(std::distance(_players.begin(), iter));

        std::ostringstream oss;

        if (winner_pos >= _players.size())
        {
            oss << "Calculated winner position " << winner_pos << " is not valid as there are " << _players.size() << " players";
            throw std::logic_error(oss.str());
        }

        oss << "Player in pos " << winner_pos << " won the game as everyone else folded.";

        _user_interaction.notify_player(oss.str());
    }
    else
    {
        execute_showdown();
    }

    // Move the dealer button around
    std::rotate(_players.begin(), std::next(_players.begin()), _players.end());
}

bool texas_holdem_game::run_betting_round()
{
    auto checks_or_folds_needed = get_active_player_count();

    for (auto curr_player_pos = 2 % _players.size();
         (get_active_player_count() > 1) && (checks_or_folds_needed > 0);
         curr_player_pos = (curr_player_pos + 1) % _players.size())
    {
        auto& player = _players.at(curr_player_pos);
        if (player.has_folded() || player.is_all_in)
        {
            continue;
        }

        const bool needs_to_invest = player.investment_in_pot < _max_investment;
        const auto amount_needed_to_call = needs_to_invest ? (_max_investment - player.investment_in_pot) : 0;

        if (player.is_our_user)
        {
            const auto recommended_action = player_action_check_or_call{}; // TODO
            if (needs_to_invest)
            {
                std::ostringstream oss;
                oss << "It's your turn and you need to invest " << amount_needed_to_call << " to call";
                _user_interaction.notify_player(oss.str());
            }
            player.actions_taken.emplace_back(_user_interaction.get_user_action(curr_player_pos, recommended_action));
        }
        else
        {
            player.actions_taken.emplace_back(_user_interaction.get_opponent_action(curr_player_pos));
        }

        auto act_on_action = [&](const auto& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, player_action_fold>)
            {
                --checks_or_folds_needed;
            }
            else if constexpr (std::is_same_v<T, player_action_check_or_call>)
            {
                --checks_or_folds_needed;
                player.investment_in_pot += amount_needed_to_call;
                _pot_size += amount_needed_to_call;
            }
            else if constexpr (std::is_same_v<T, player_action_raise>)
            {
                checks_or_folds_needed = get_active_player_count() - 1; // Players need to react to this raise

                const auto investment = amount_needed_to_call + arg.amount_raised_above_call;
                player.investment_in_pot += investment;
                _pot_size += investment;
            }
            else
            {
                static_assert(always_false<T>::value, "non-exhaustive visitor!");
            }
        };
        std::visit(act_on_action, player.actions_taken.back());
    }

    return get_active_player_count() < 2;
}

void texas_holdem_game::execute_showdown()
{
    // TODO
    _user_interaction.notify_player("Not yet implemented");
}

size_t texas_holdem_game::get_active_player_count() const
{
    return static_cast<size_t>(std::count_if(_players.begin(),
                                             _players.end(),
                                             [](const player_state& state){ return !state.has_folded(); }));
}

} // end of namespace poker_lib
