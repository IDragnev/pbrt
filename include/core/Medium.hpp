#pragma once

namespace idragnev::pbrt {
    class Medium;

    struct MediumInterface
    {
        MediumInterface() = default;
        MediumInterface(const Medium* m) : inside(m), outside(m) {}
        MediumInterface(const Medium* inside, const Medium* outside)
            : inside(inside)
            , outside(outside) {}

        bool isMediumTransition() const { return inside != outside; }

        const Medium* inside = nullptr;
        const Medium* outside = nullptr;
    };
} // namespace idragnev::pbrt