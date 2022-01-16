#include "pbrt/core/reflection/Utility.hpp"

namespace idragnev::pbrt::reflection {
    Float cosDPhi(const Vector3f& wa, const Vector3f& wb) {
        const Float waxy = wa.x * wa.x + wa.y * wa.y;
        const Float wbxy = wb.x * wb.x + wb.y * wb.y;
        if (waxy == 0.f || wbxy == 0.f) {
            return 1.f;
        }

        return clamp((wa.x * wb.x + wa.y * wb.y) / std::sqrt(waxy * wbxy),
                     Float(-1),
                     Float(1));
    }

    Float FresnelReflDielectric(Float cosThetaI, Float etaI, Float etaT) {
        cosThetaI = clamp(cosThetaI, Float(-1), Float(1));

        if (const bool entering = cosThetaI > 0.f; entering == false) {
            std::swap(etaI, etaT);
            cosThetaI = std::abs(cosThetaI);
        }

        // compute cosThetaT using Snell's law
        const Float sinThetaI =
            std::sqrt(std::max(Float(0), Float(1) - cosThetaI * cosThetaI));
        const Float sinThetaT = etaI / etaT * sinThetaI;

        if (sinThetaT >= Float(1)) {
            // total internal reflection
            return 1.f;
        }

        const Float cosThetaT =
            std::sqrt(std::max(Float(0), Float(1) - sinThetaT * sinThetaT));
        const Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
                            ((etaT * cosThetaI) + (etaI * cosThetaT));
        const Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                            ((etaI * cosThetaI) + (etaT * cosThetaT));

        return (Rparl * Rparl + Rperp * Rperp) / 2;
    }
} // namespace idragnev::pbrt::reflection