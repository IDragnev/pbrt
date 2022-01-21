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

    Float fresnelReflDielectric(Float cosThetaI, Float etaI, Float etaT) {
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

    // https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    Spectrum fresnelReflConductor(Float cosThetaI,
                                  const Spectrum& etaI,
                                  const Spectrum& etaT,
                                  const Spectrum& k) {
        cosThetaI = clamp(cosThetaI, Float(-1), Float(1));

        const Spectrum eta = etaT / etaI;
        const Spectrum etak = k / etaI;

        const Float cosThetaI2 = cosThetaI * cosThetaI;
        const Float sinThetaI2 = Float(1) - cosThetaI2;
        const Spectrum eta2 = eta * eta;
        const Spectrum etak2 = etak * etak;

        const Spectrum t0 = eta2 - etak2 - Spectrum(sinThetaI2);
        const Spectrum a2plusb2 = sqrt(t0 * t0 + (Float(4) * eta2 * etak2));
        const Spectrum t1 = a2plusb2 + Spectrum(cosThetaI2);
        const Spectrum a = sqrt(0.5f * (a2plusb2 + t0));
        const Spectrum t2 = Float(2) * cosThetaI * a;
        const Spectrum Rs = (t1 - t2) / (t1 + t2);

        const Spectrum t3 =
            cosThetaI2 * a2plusb2 + Spectrum(sinThetaI2) * sinThetaI2;
        const Spectrum t4 = t2 * sinThetaI2;
        const Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

        return 0.5 * (Rp + Rs);
    }
} // namespace idragnev::pbrt::reflection