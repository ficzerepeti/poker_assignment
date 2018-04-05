#pragma once

#include "i_analysis_engine_factory.h"
#include "i_my_poker_lib.h"

namespace poker_lib {

class analysis_engine_factory : public i_analysis_engine_factory
{
public:
    explicit analysis_engine_factory(i_my_poker_lib &poker_lib);
    ~analysis_engine_factory() override = default;

    std::unique_ptr<i_analysis_engine> make_analysis_engine(size_t num_of_players) override;

private:
    i_my_poker_lib& _poker_lib;
};

} // end of namespace poker_lib
