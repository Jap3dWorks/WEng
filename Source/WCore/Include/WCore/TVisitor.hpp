#pragma once

#include <variant>

namespace wcr {
    template<typename ...Args>
    struct TVisitor : Args ... {
        using Args::operator() ...;
    };

    template<class... Ts>
    TVisitor(Ts...) -> TVisitor<Ts...>;
}
