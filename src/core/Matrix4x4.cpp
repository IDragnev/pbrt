#include "core/Matrix4x4.hpp"
#include "core/Vector3.hpp"

#include <assert.h>
#include <utility>
#include <cstring>

namespace idragnev::pbrt {
    Matrix4x4::Matrix4x4(const Float mat[4][4]) {
        std::memcpy(m, mat, 16 * sizeof(Float));
    }

    const Matrix4x4& Matrix4x4::identity() noexcept {
        static Matrix4x4 id;
        return id;
    }

    Vector3f translationVector(const Matrix4x4& matrix) noexcept {
        return {matrix.m[0][3], matrix.m[1][3], matrix.m[2][3]};
    }

    bool operator==(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
        for (auto i = 0u; i < 4u; ++i) {
            for (auto j = 0u; j < 4u; ++j) {
                if (lhs.m[i][j] != rhs.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    Matrix4x4 mul(const Matrix4x4& m1, const Matrix4x4& m2) noexcept {
        Matrix4x4 result;
        for (auto i = 0u; i < 4u; ++i) {
            for (auto j = 0u; j < 4u; ++j) {
                // clang-format off
                result.m[i][j] =
                    m1.m[i][0] * m2.m[0][j] + 
                    m1.m[i][1] * m2.m[1][j] +
                    m1.m[i][2] * m2.m[2][j] + 
                    m1.m[i][3] * m2.m[3][j];
                // clang-format on
            }
        }
        return result;
    }

    Matrix4x4 transpose(const Matrix4x4& matrix) noexcept {
        // clang-format off
        return Matrix4x4{
            matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0], 
            matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1], 
            matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2], 
            matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]
        };
        // clang-format on
    }

    Matrix4x4 inverse(const Matrix4x4& matrix) {
        std::size_t indxc[4];
        std::size_t indxr[4];
        std::size_t ipiv[4] = {0, 0, 0, 0};
        Float mInverse[4][4];
        memcpy(mInverse, matrix.m, 16 * sizeof(Float));

        for (auto i = 0u; i < 4u; i++) {
            std::size_t irow = 0;
            std::size_t icol = 0;
            Float big = 0.0f;

            // Choose pivot
            for (auto j = 0u; j < 4u; j++) {
                if (ipiv[j] != 1u) {
                    for (auto k = 0u; k < 4u; k++) {
                        if (ipiv[k] == 0u) {
                            if (std::abs(mInverse[j][k]) >= big) {
                                big = Float(std::abs(mInverse[j][k]));
                                irow = j;
                                icol = k;
                            }
                        }
                        else {
                            assert(ipiv[k] <= 1);
                        }
                    }
                }
            }

            ++ipiv[icol];
            // Swap rows _irow_ and _icol_ for pivot
            if (irow != icol) {
                for (auto k = 0u; k < 4u; ++k) {
                    std::swap(mInverse[irow][k], mInverse[icol][k]);
                }
            }
            indxr[i] = irow;
            indxc[i] = icol;
            assert(mInverse[icol][icol] != 0.f);

            // Set m[icol][icol] to one by scaling row _icol_ appropriately
            Float pivinv = 1.0f / mInverse[icol][icol];
            mInverse[icol][icol] = 1.0f;
            for (auto j = 0u; j < 4u; j++) {
                mInverse[icol][j] *= pivinv;
            }

            // Subtract this row from others to zero out their columns
            for (auto j = 0u; j < 4u; j++) {
                if (j != icol) {
                    Float save = mInverse[j][icol];
                    mInverse[j][icol] = 0.0f;
                    for (auto k = 0u; k < 4u; k++) {
                        mInverse[j][k] -= mInverse[icol][k] * save;
                    }
                }
            }
        }

        // Swap columns to reflect permutation
        for (int j = 3; j >= 0; j--) {
            if (indxr[j] != indxc[j]) {
                for (auto k = 0u; k < 4u; k++) {
                    std::swap(mInverse[k][indxr[j]], mInverse[k][indxc[j]]);
                }
            }
        }

        return Matrix4x4{mInverse};
    }
} // namespace idragnev::pbrt