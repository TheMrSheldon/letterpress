#pragma once

namespace lp::utils {
    /** Taken from https://en.cppreference.com/w/cpp/utility/variant/visit **/
    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };
}