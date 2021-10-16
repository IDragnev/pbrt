#include "pbrt/filters/Sinc.hpp"

namespace idragnev::pbrt::filters {
    Float LanczosSincFilter::eval(const Point2f& p) const {
        return windowedSinc(p.x, radius.x) * windowedSinc(p.y, radius.y);
    }

    Float LanczosSincFilter::windowedSinc(Float x, const Float r) const {
        x = std::abs(x);

        if (x > r) {
            return 0.f;
        }
        else {
            assert(tau > Float(0));

            const Float lanczos = sinc(x / tau);
            return sinc(x) * lanczos;
        }
    }

    Float LanczosSincFilter::sinc(Float x) const {
        x = std::abs(x);

        if (x < 1e-5) {
            return 1.f;
        }
        else {
            using math::constants::Pi;

            return std::sin(Pi * x) / (Pi * x);
        }
    }
} // namespace idragnev::pbrt::filters