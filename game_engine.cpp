#include <sstream>
#include "game_engine.h"

namespace poker_lib {

game_engine::game_engine(i_analysis_engine_factory& analysis_engine_factory,
                         const size_t num_of_players,
                         const size_t user_position)
:
    _analysis_engine_factory(analysis_engine_factory),
    _players(num_of_players),
    _user_position(user_position),
    _analysis_engine(_analysis_engine_factory.make_analysis_engine(num_of_players))
{
    if (_players.size() < 2)
    {
        throw std::invalid_argument("Cannot play poker with less than two players at the table");
    }
    if (_players.size() <= user_position)
    {
        std::ostringstream oss;
        oss << "User position is set as " << user_position << " which is not possible at a table size of " << _players.size();
        throw std::invalid_argument(oss.str());
    }
}

operation_status game_engine::set_pocket_cards(const std::string &cards)
{
    if (!_analysis_engine->set_player_pocket_cards(_user_position, cards))
    {
        return operation_status::invalid_format;
    }

    _pocket_cards = cards;
    return operation_status::success; // TODO: follow phase of the game and return status accordingly
}

operation_status game_engine::set_flop(const std::string &cards)
{
    if (!_analysis_engine->set_board(cards + _turn + _river))
    {
        return operation_status::invalid_format;
    }

    _flop = cards;
    return operation_status::success; // TODO: follow phase of the game and return status accordingly
}

operation_status game_engine::set_turn(const std::string &card)
{
    if (!_analysis_engine->set_board(_flop + card + _river))
    {
        return operation_status::invalid_format;
    }

    _turn = card;
    return operation_status::success; // TODO: follow phase of the game and return status accordingly
}

operation_status game_engine::set_river(const std::string &card)
{
    if (!_analysis_engine->set_board(_flop + _turn + card))
    {
        return operation_status::invalid_format;
    }

    _river = card;
    return operation_status::success; // TODO: follow phase of the game and return status accordingly
}

operation_status game_engine::set_user_action(const player_action_t &action)
{
    _players.at(_user_position).actions_taken.emplace_back(action);
    return operation_status::success;
}

operation_status game_engine::set_opponent_action(size_t position, const player_action_t &action)
{
    _players.at(position).actions_taken.emplace_back(action);
    return operation_status::success;
}

user_analysis game_engine::make_user_analysis()
{
    return _analysis_engine->make_user_analysis(_user_position);
}

} // end of namespace poker_lib
