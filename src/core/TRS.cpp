#include "TRS.hpp"
#include "Matrix4x4.hpp"
#include "Transformation.hpp"

#include <algorithm>

namespace idragnev::pbrt {
    TRS decompose(const Matrix4x4& matrix) {
        const auto T = translationVector(matrix);
        const auto RS = detail::removeTranslation(matrix);
        const auto R = detail::extractRotation(RS);

        TRS result;
        result.translation = T;
        result.rotation = Quaternion{Transformation{R}};
        result.scaling = inverse(R) * RS;

        return result;
    }

    namespace detail {
        Matrix4x4 removeTranslation(const Matrix4x4& TRS) noexcept {
            auto RS = TRS;

            RS.m[0][3] = 0.f;
            RS.m[1][3] = 0.f;
            RS.m[2][3] = 0.f;
            RS.m[3][3] = 1.f;

            return RS;
        }

        //assumes M has no translation
        Matrix4x4 extractRotation(const Matrix4x4& RS) {
            Float normDifference = 0.f;
            auto count = 0;
            auto R = RS;
            
            do {
                const auto Rnext = computeRNext(R);
                normDifference = computeNormDifference(R, Rnext);
                
                R = Rnext;
                ++count;
            } while (count < 100 && normDifference > .0001);
            
            return R;
        }

        Matrix4x4 computeRNext(const Matrix4x4& R) {
            auto RNext = Matrix4x4{};

            const auto Rit = inverse(transpose(R));
            for (auto i = 0; i < 4; ++i) {
                for (auto j = 0; j < 4; ++j) {
                    RNext.m[i][j] = 0.5f * (R.m[i][j] + Rit.m[i][j]);
                }
            }

            return RNext;
        }

        Float computeNormDifference(const Matrix4x4& R, const Matrix4x4& Rnext) {
            Float norm = 0.f;
            for (auto i = 0; i < 3; ++i) {
                const Float n = 
                    std::abs(R.m[i][0] - Rnext.m[i][0]) +
                    std::abs(R.m[i][1] - Rnext.m[i][1]) +
                    std::abs(R.m[i][2] - Rnext.m[i][2]);
                norm = std::max(norm, n);
            }

            return norm;
        }
    } //namespace detail
} //namespace idragnev::pbrt

