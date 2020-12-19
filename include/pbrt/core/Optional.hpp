#pragma once

#include <tl/optional.hpp>

namespace idragnev::pbrt {
    template <typename T>
    using Optional = tl::optional<T>;

    using tl::make_optional;
    using tl::nullopt;
} // namespace idragnev::pbrt