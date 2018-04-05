#pragma once

#include <memory>
#include "i_analysis_engine.h"

namespace poker_lib {

class i_analysis_engine_factory
{
public:
    virtual ~i_analysis_engine_factory() = default;

    virtual std::unique_ptr<i_analysis_engine> make_analysis_engine(size_t num_of_players) = 0;
};

} // end of namespace poker_lib
