#include "core/RayDifferential.hpp"

namespace idragnev::pbrt {
    bool RayDifferential::hasNaNs() const {
        return Ray::hasNaNs() ||
            (hasDifferentials &&
             (rxOrigin.hasNaNs() || ryOrigin.hasNaNs() || rxDirection.hasNaNs() || ryDirection.hasNaNs())
            );
    }

    void RayDifferential::scaleDifferentials(Float s) {
        rxOrigin = o + s * (rxOrigin - o);
        ryOrigin = o + s * (ryOrigin - o);
        rxDirection = d + s * (rxDirection - d);
        ryDirection = d + s * (ryDirection - d);
    }
} //namespace idragnev::pbrt 