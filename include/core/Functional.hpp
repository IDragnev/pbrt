#pragma once

#include <type_traits>

namespace idragnev::pbrt {
    template <template <typename...> typename C,
              typename T,
              typename F,
              typename R = C<std::invoke_result_t<F, T>>>
    R fmap(const C<T>& c, F f) {
        R result;

        for (const auto& e : c) {
            result.push_back(f(e));
        }

        return result;
    }
} // namespace idragnev::pbrt