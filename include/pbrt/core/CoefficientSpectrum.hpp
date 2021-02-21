#pragma once

#include "pbrt/core/core.hpp"

#include <assert.h>
#include <array>
#include <algorithm>

namespace idragnev::pbrt {
    template <std::size_t SamplesCount>
    class CoefficientSpectrum
    {
        static_assert(SamplesCount > 0);

    public:
        inline static constexpr auto NumberOfSamples = SamplesCount;

        CoefficientSpectrum() = default;
        explicit CoefficientSpectrum(
            const std::array<Float, SamplesCount>& samples)
            : samples(samples) {}
        explicit CoefficientSpectrum(const Float v) {
            assert(isNaN(v) == false);
            samples.fill(v);
        }

        template <typename BinOp>
        CoefficientSpectrum zipWith(const CoefficientSpectrum& other,
                                    BinOp op) const;
        template <typename F>
        CoefficientSpectrum map(F f) const;

        CoefficientSpectrum& operator+=(const CoefficientSpectrum& rhs) {
            *this = this->zipWith(rhs, std::plus{});
            return *this;
        }

        CoefficientSpectrum& operator-=(const CoefficientSpectrum& rhs) {
            *this = this->zipWith(rhs, std::minus{});
            return *this;
        }

        CoefficientSpectrum& operator*=(const CoefficientSpectrum& rhs) {
            *this = this->zipWith(rhs, std::multiplies{});
            return *this;
        }

        CoefficientSpectrum& operator*=(const Float v) {
            assert(isNaN(v) == false);
            *this = this->map([v](const Float s) { return s * v; });

            return *this;
        }

        CoefficientSpectrum& operator/=(const Float v) {
            assert(isNaN(v) == false);
            assert(v != 0.f);
            *this = this->map([v](const Float s) { return s / v; });

            return *this;
        }

        bool operator==(const CoefficientSpectrum& rhs) const;
        bool operator!=(const CoefficientSpectrum& rhs) const {
            return !(*this == rhs);
        }

        bool isBlack() const {
            return std::all_of(this->samples.cbegin(),
                               this->samples.cend(),
                               [](const Float s) { return s == 0.f; });
        }

        Float maxComponentValue() const;

        bool hasNaNs() const {
            return std::any_of(this->samples.cbegin(),
                               this->samples.cend(),
                               [](const Float s) { return isNaN(s); });
        }

        Float& operator[](const std::size_t i) {
            assert(i >= 0 && i < SamplesCount);
            return this->samples[i];
        }

        Float operator[](const std::size_t i) const {
            assert(i >= 0 && i < SamplesCount);
            return this->samples[i];
        }

    private:
        std::array<Float, SamplesCount> samples;
    };

    template <std::size_t SamplesCount>
    CoefficientSpectrum<SamplesCount>
    operator+(const CoefficientSpectrum<SamplesCount>& lhs,
              const CoefficientSpectrum<SamplesCount>& rhs) {
        auto result = lhs;
        result += rhs;

        return result;
    }

    template <std::size_t SamplesCount>
    CoefficientSpectrum<SamplesCount>
    operator-(const CoefficientSpectrum<SamplesCount>& lhs,
              const CoefficientSpectrum<SamplesCount>& rhs) {
        auto result = lhs;
        result -= rhs;

        return result;
    }

    template <std::size_t SamplesCount>
    CoefficientSpectrum<SamplesCount>
    operator/(const CoefficientSpectrum<SamplesCount>& lhs,
              const CoefficientSpectrum<SamplesCount>& rhs) {
        auto result = lhs;
        result /= rhs;

        return result;
    }

    template <std::size_t SamplesCount>
    CoefficientSpectrum<SamplesCount>
    operator*(const CoefficientSpectrum<SamplesCount>& lhs,
              const CoefficientSpectrum<SamplesCount>& rhs) {
        auto result = lhs;
        result *= rhs;

        return result;
    }

    template <std::size_t SamplesCount>
    CoefficientSpectrum<SamplesCount>
    operator*(const CoefficientSpectrum<SamplesCount>& cs, const Float v) {
        auto result = cs;
        result *= v;

        return result;
    }

    template <std::size_t SamplesCount>
    inline CoefficientSpectrum<SamplesCount>
    operator*(const Float v, const CoefficientSpectrum<SamplesCount>& cs) {
        return cs * v;
    }

    template <std::size_t SamplesCount>
    inline CoefficientSpectrum<SamplesCount>
    operator-(const CoefficientSpectrum<SamplesCount>& cs) {
        return -1.f * cs;
    }

    template <std::size_t SamplesCount>
    inline CoefficientSpectrum<SamplesCount>
    sqrt(const CoefficientSpectrum<SamplesCount>& cs) {
        auto result = cs.map([](const Float s) { return std::sqrt(s); });
        assert(result.hasNaNs() == false);

        return result;
    }

    template <std::size_t SamplesCount>
    template <typename BinOp>
    CoefficientSpectrum<SamplesCount>
    CoefficientSpectrum<SamplesCount>::zipWith(const CoefficientSpectrum& other,
                                               BinOp op) const {
        static_assert(std::is_invocable_r_v<Float, BinOp, Float, Float>,
                      "op must have the signature Float(Float, Float)");
        assert(other.hasNaNs() == false);

        CoefficientSpectrum result;
        for (std::size_t i = 0; i < SamplesCount; ++i) {
            result.samples[i] = op(this->samples[i], other.samples[i]);
        }

        assert(result.hasNaNs() == false);

        return result;
    }

    template <std::size_t SamplesCount>
    template <typename F>
    CoefficientSpectrum<SamplesCount>
    CoefficientSpectrum<SamplesCount>::map(F f) const {
        static_assert(std::is_invocable_r_v<Float, F, Float>,
                      "f must have the signature Float(Float)");

        auto result = *this;
        for (Float& s : result->samples) {
            s = f(s);
        }

        assert(result.hasNaNs() == false);
        return result;
    }

    template <std::size_t SamplesCount>
    inline CoefficientSpectrum<SamplesCount>
    pow(const CoefficientSpectrum<SamplesCount>& cs, const Float e) {
        return cs.map([e](const Float s) { return std::pow(s, e); });
    }

    template <std::size_t SamplesCount>
    inline CoefficientSpectrum<SamplesCount>
    exp(const CoefficientSpectrum<SamplesCount>& cs) {
        return cs.map([](const Float s) { return std::exp(s); });
    }

    template <std::size_t SamplesCount>
    inline CoefficientSpectrum<SamplesCount>
    clamp(const CoefficientSpectrum<SamplesCount>& cs,
          const Float low,
          const Float high) {
        return cs.map([=](const Float s) { return pbrt::clamp(s, low, high); });
    }

    template <std::size_t SamplesCount>
    bool CoefficientSpectrum<SamplesCount>::operator==(
        const CoefficientSpectrum& rhs) const {
        for (std::size_t i = 0; i < SamplesCount; ++i) {
            if (this->samples[i] != rhs.samples[i]) {
                return false;
            }
        }

        return true;
    }

    template <std::size_t SamplesCount>
    Float CoefficientSpectrum<SamplesCount>::maxComponentValue() const {
        Float m = samples[0];
        for (std::size_t i = 1; i < SamplesCount; ++i) {
            m = std::max(m, this->samples[i]);
        }

        return m;
    }
} // namespace idragnev::pbrt