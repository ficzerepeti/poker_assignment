#include "analysis_engine_factory.h"
#include "analysis_engine.h"

namespace poker_lib {

analysis_engine_factory::analysis_engine_factory(i_my_poker_lib &poker_lib)
:
    _poker_lib(poker_lib)
{}

std::unique_ptr<i_analysis_engine> analysis_engine_factory::make_analysis_engine(size_t num_of_players)
{
    return std::make_unique<analysis_engine>(_poker_lib, num_of_players);
}

} // end of namespace poker_lib
