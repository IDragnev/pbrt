#pragma once

#include <optional>

namespace idragnev::pbrt {
    template <typename T>
    using Optional = std::optional<T>;

    using std::make_optional;
    using std::nullopt;
} // namespace idragnev::pbrt