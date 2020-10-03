#include "core/AnimatedTransformation.hpp"
#include "core/Transformation.hpp"
#include "core/TRS.hpp"
#include "core/Ray.hpp"
#include "core/RayDifferential.hpp"
#include "core/Bounds3.hpp"
#include "core/Interval.hpp"

namespace idragnev::pbrt {
    AnimatedTransformation::AnimatedTransformation(
        const Transformation& startTransform,
        const Float startTime,
        const Transformation& endTransform,
        const Float endTime)
        : startTransform{&startTransform}
        , endTransform{&endTransform}
        , startTime{startTime}
        , endTime{endTime}
        , actuallyAnimated{startTransform != endTransform} {
        if (!actuallyAnimated) {
            return;
        }

        startTRS = decompose(startTransform.matrix());
        endTRS = decompose(endTransform.matrix());

        const auto rotationsDot = dot(startTRS.R, endTRS.R);
        if (const auto isAngleObtuse = rotationsDot < 0.f; isAngleObtuse) {
            endTRS.R = -endTRS.R;
        }
        hasRotation = rotationsDot < 0.9995f;

        if (hasRotation) {
            const auto cosTheta = dot(startTRS.R, endTRS.R);
            const auto theta = std::acos(clamp(cosTheta, -1.f, 1.f));
            const auto qperp = normalize(endTRS.R - startTRS.R * cosTheta);

            const auto t0x = startTRS.T.x;
            const auto t0y = startTRS.T.y;
            const auto t0z = startTRS.T.z;
            const auto t1x = endTRS.T.x;
            const auto t1y = endTRS.T.y;
            const auto t1z = endTRS.T.z;
            const auto q0x = startTRS.R.v.x;
            const auto q0y = startTRS.R.v.y;
            const auto q0z = startTRS.R.v.z;
            const auto q0w = startTRS.R.w;
            const auto qperpx = qperp.v.x;
            const auto qperpy = qperp.v.y;
            const auto qperpz = qperp.v.z;
            const auto qperpw = qperp.w;
            const auto s000 = startTRS.S.m[0][0];
            const auto s001 = startTRS.S.m[0][1];
            const auto s002 = startTRS.S.m[0][2];
            const auto s010 = startTRS.S.m[1][0];
            const auto s011 = startTRS.S.m[1][1];
            const auto s012 = startTRS.S.m[1][2];
            const auto s020 = startTRS.S.m[2][0];
            const auto s021 = startTRS.S.m[2][1];
            const auto s022 = startTRS.S.m[2][2];
            const auto s100 = endTRS.S.m[0][0];
            const auto s101 = endTRS.S.m[0][1];
            const auto s102 = endTRS.S.m[0][2];
            const auto s110 = endTRS.S.m[1][0];
            const auto s111 = endTRS.S.m[1][1];
            const auto s112 = endTRS.S.m[1][2];
            const auto s120 = endTRS.S.m[2][0];
            const auto s121 = endTRS.S.m[2][1];
            const auto s122 = endTRS.S.m[2][2];

            // clang-format off
            c1[0] = DerivativeTerm{
                -t0x + t1x,
                (-1 + q0y * q0y + q0z * q0z + qperpy * qperpy + qperpz * qperpz) *
                s000 +
                q0w * q0z * s010 - qperpx * qperpy * s010 +
                qperpw * qperpz * s010 - q0w * q0y * s020 -
                qperpw * qperpy * s020 - qperpx * qperpz * s020 + s100 -
                q0y * q0y * s100 - q0z * q0z * s100 - qperpy * qperpy * s100 -
                qperpz * qperpz * s100 - q0w * q0z * s110 +
                qperpx * qperpy * s110 - qperpw * qperpz * s110 +
                q0w * q0y * s120 + qperpw * qperpy * s120 +
                qperpx * qperpz * s120 +
                q0x * (-(q0y * s010) - q0z * s020 + q0y * s110 + q0z * s120),
                (-1 + q0y * q0y + q0z * q0z + qperpy * qperpy + qperpz * qperpz) *
                s001 +
                q0w * q0z * s011 - qperpx * qperpy * s011 +
                qperpw * qperpz * s011 - q0w * q0y * s021 -
                qperpw * qperpy * s021 - qperpx * qperpz * s021 + s101 -
                q0y * q0y * s101 - q0z * q0z * s101 - qperpy * qperpy * s101 -
                qperpz * qperpz * s101 - q0w * q0z * s111 +
                qperpx * qperpy * s111 - qperpw * qperpz * s111 +
                q0w * q0y * s121 + qperpw * qperpy * s121 +
                qperpx * qperpz * s121 +
                q0x * (-(q0y * s011) - q0z * s021 + q0y * s111 + q0z * s121),
                (-1 + q0y * q0y + q0z * q0z + qperpy * qperpy + qperpz * qperpz) *
                s002 +
                q0w * q0z * s012 - qperpx * qperpy * s012 +
                qperpw * qperpz * s012 - q0w * q0y * s022 -
                qperpw * qperpy * s022 - qperpx * qperpz * s022 + s102 -
                q0y * q0y * s102 - q0z * q0z * s102 - qperpy * qperpy * s102 -
                qperpz * qperpz * s102 - q0w * q0z * s112 +
                qperpx * qperpy * s112 - qperpw * qperpz * s112 +
                q0w * q0y * s122 + qperpw * qperpy * s122 +
                qperpx * qperpz * s122 +
                q0x * (-(q0y * s012) - q0z * s022 + q0y * s112 + q0z * s122)
            };

            c2[0] = DerivativeTerm{
                0.,
                -(qperpy * qperpy * s000) - qperpz * qperpz * s000 +
                qperpx * qperpy * s010 - qperpw * qperpz * s010 +
                qperpw * qperpy * s020 + qperpx * qperpz * s020 +
                q0y * q0y * (s000 - s100) + q0z * q0z * (s000 - s100) +
                qperpy * qperpy * s100 + qperpz * qperpz * s100 -
                qperpx * qperpy * s110 + qperpw * qperpz * s110 -
                qperpw * qperpy * s120 - qperpx * qperpz * s120 +
                2 * q0x * qperpy * s010 * theta -
                2 * q0w * qperpz * s010 * theta +
                2 * q0w * qperpy * s020 * theta +
                2 * q0x * qperpz * s020 * theta +
                q0y *
                (q0x * (-s010 + s110) + q0w * (-s020 + s120) +
                    2 * (-2 * qperpy * s000 + qperpx * s010 + qperpw * s020) *
                    theta) +
                q0z * (q0w * (s010 - s110) + q0x * (-s020 + s120) -
                    2 * (2 * qperpz * s000 + qperpw * s010 - qperpx * s020) *
                    theta),
                -(qperpy * qperpy * s001) - qperpz * qperpz * s001 +
                qperpx * qperpy * s011 - qperpw * qperpz * s011 +
                qperpw * qperpy * s021 + qperpx * qperpz * s021 +
                q0y * q0y * (s001 - s101) + q0z * q0z * (s001 - s101) +
                qperpy * qperpy * s101 + qperpz * qperpz * s101 -
                qperpx * qperpy * s111 + qperpw * qperpz * s111 -
                qperpw * qperpy * s121 - qperpx * qperpz * s121 +
                2 * q0x * qperpy * s011 * theta -
                2 * q0w * qperpz * s011 * theta +
                2 * q0w * qperpy * s021 * theta +
                2 * q0x * qperpz * s021 * theta +
                q0y *
                (q0x * (-s011 + s111) + q0w * (-s021 + s121) +
                    2 * (-2 * qperpy * s001 + qperpx * s011 + qperpw * s021) *
                    theta) +
                q0z * (q0w * (s011 - s111) + q0x * (-s021 + s121) -
                    2 * (2 * qperpz * s001 + qperpw * s011 - qperpx * s021) *
                    theta),
                -(qperpy * qperpy * s002) - qperpz * qperpz * s002 +
                qperpx * qperpy * s012 - qperpw * qperpz * s012 +
                qperpw * qperpy * s022 + qperpx * qperpz * s022 +
                q0y * q0y * (s002 - s102) + q0z * q0z * (s002 - s102) +
                qperpy * qperpy * s102 + qperpz * qperpz * s102 -
                qperpx * qperpy * s112 + qperpw * qperpz * s112 -
                qperpw * qperpy * s122 - qperpx * qperpz * s122 +
                2 * q0x * qperpy * s012 * theta -
                2 * q0w * qperpz * s012 * theta +
                2 * q0w * qperpy * s022 * theta +
                2 * q0x * qperpz * s022 * theta +
                q0y *
                (q0x * (-s012 + s112) + q0w * (-s022 + s122) +
                    2 * (-2 * qperpy * s002 + qperpx * s012 + qperpw * s022) *
                    theta) +
                q0z * (q0w * (s012 - s112) + q0x * (-s022 + s122) -
                    2 * (2 * qperpz * s002 + qperpw * s012 - qperpx * s022) *
                    theta)
            };

            c3[0] = DerivativeTerm{
                0.,
                -2 * (q0x * qperpy * s010 - q0w * qperpz * s010 +
                    q0w * qperpy * s020 + q0x * qperpz * s020 -
                    q0x * qperpy * s110 + q0w * qperpz * s110 -
                    q0w * qperpy * s120 - q0x * qperpz * s120 +
                    q0y * (-2 * qperpy * s000 + qperpx * s010 + qperpw * s020 +
                        2 * qperpy * s100 - qperpx * s110 - qperpw * s120) +
                    q0z * (-2 * qperpz * s000 - qperpw * s010 + qperpx * s020 +
                        2 * qperpz * s100 + qperpw * s110 - qperpx * s120)) *
                theta,
                -2 * (q0x * qperpy * s011 - q0w * qperpz * s011 +
                    q0w * qperpy * s021 + q0x * qperpz * s021 -
                    q0x * qperpy * s111 + q0w * qperpz * s111 -
                    q0w * qperpy * s121 - q0x * qperpz * s121 +
                    q0y * (-2 * qperpy * s001 + qperpx * s011 + qperpw * s021 +
                        2 * qperpy * s101 - qperpx * s111 - qperpw * s121) +
                    q0z * (-2 * qperpz * s001 - qperpw * s011 + qperpx * s021 +
                        2 * qperpz * s101 + qperpw * s111 - qperpx * s121)) *
                theta,
                -2 * (q0x * qperpy * s012 - q0w * qperpz * s012 +
                    q0w * qperpy * s022 + q0x * qperpz * s022 -
                    q0x * qperpy * s112 + q0w * qperpz * s112 -
                    q0w * qperpy * s122 - q0x * qperpz * s122 +
                    q0y * (-2 * qperpy * s002 + qperpx * s012 + qperpw * s022 +
                        2 * qperpy * s102 - qperpx * s112 - qperpw * s122) +
                    q0z * (-2 * qperpz * s002 - qperpw * s012 + qperpx * s022 +
                        2 * qperpz * s102 + qperpw * s112 - qperpx * s122)) *
                theta
            };

            c4[0] = DerivativeTerm{
                0.,
                -(q0x * qperpy * s010) + q0w * qperpz * s010 - q0w * qperpy * s020 -
                q0x * qperpz * s020 + q0x * qperpy * s110 -
                q0w * qperpz * s110 + q0w * qperpy * s120 +
                q0x * qperpz * s120 + 2 * q0y * q0y * s000 * theta +
                2 * q0z * q0z * s000 * theta -
                2 * qperpy * qperpy * s000 * theta -
                2 * qperpz * qperpz * s000 * theta +
                2 * qperpx * qperpy * s010 * theta -
                2 * qperpw * qperpz * s010 * theta +
                2 * qperpw * qperpy * s020 * theta +
                2 * qperpx * qperpz * s020 * theta +
                q0y * (-(qperpx * s010) - qperpw * s020 +
                    2 * qperpy * (s000 - s100) + qperpx * s110 +
                    qperpw * s120 - 2 * q0x * s010 * theta -
                    2 * q0w * s020 * theta) +
                q0z * (2 * qperpz * s000 + qperpw * s010 - qperpx * s020 -
                    2 * qperpz * s100 - qperpw * s110 + qperpx * s120 +
                    2 * q0w * s010 * theta - 2 * q0x * s020 * theta),
                -(q0x * qperpy * s011) + q0w * qperpz * s011 - q0w * qperpy * s021 -
                q0x * qperpz * s021 + q0x * qperpy * s111 -
                q0w * qperpz * s111 + q0w * qperpy * s121 +
                q0x * qperpz * s121 + 2 * q0y * q0y * s001 * theta +
                2 * q0z * q0z * s001 * theta -
                2 * qperpy * qperpy * s001 * theta -
                2 * qperpz * qperpz * s001 * theta +
                2 * qperpx * qperpy * s011 * theta -
                2 * qperpw * qperpz * s011 * theta +
                2 * qperpw * qperpy * s021 * theta +
                2 * qperpx * qperpz * s021 * theta +
                q0y * (-(qperpx * s011) - qperpw * s021 +
                    2 * qperpy * (s001 - s101) + qperpx * s111 +
                    qperpw * s121 - 2 * q0x * s011 * theta -
                    2 * q0w * s021 * theta) +
                q0z * (2 * qperpz * s001 + qperpw * s011 - qperpx * s021 -
                    2 * qperpz * s101 - qperpw * s111 + qperpx * s121 +
                    2 * q0w * s011 * theta - 2 * q0x * s021 * theta),
                -(q0x * qperpy * s012) + q0w * qperpz * s012 - q0w * qperpy * s022 -
                q0x * qperpz * s022 + q0x * qperpy * s112 -
                q0w * qperpz * s112 + q0w * qperpy * s122 +
                q0x * qperpz * s122 + 2 * q0y * q0y * s002 * theta +
                2 * q0z * q0z * s002 * theta -
                2 * qperpy * qperpy * s002 * theta -
                2 * qperpz * qperpz * s002 * theta +
                2 * qperpx * qperpy * s012 * theta -
                2 * qperpw * qperpz * s012 * theta +
                2 * qperpw * qperpy * s022 * theta +
                2 * qperpx * qperpz * s022 * theta +
                q0y * (-(qperpx * s012) - qperpw * s022 +
                    2 * qperpy * (s002 - s102) + qperpx * s112 +
                    qperpw * s122 - 2 * q0x * s012 * theta -
                    2 * q0w * s022 * theta) +
                q0z * (2 * qperpz * s002 + qperpw * s012 - qperpx * s022 -
                    2 * qperpz * s102 - qperpw * s112 + qperpx * s122 +
                    2 * q0w * s012 * theta - 2 * q0x * s022 * theta)
            };

            c5[0] = DerivativeTerm{
                0.,
                2 * (qperpy * qperpy * s000 + qperpz * qperpz * s000 -
                    qperpx * qperpy * s010 + qperpw * qperpz * s010 -
                    qperpw * qperpy * s020 - qperpx * qperpz * s020 -
                    qperpy * qperpy * s100 - qperpz * qperpz * s100 +
                    q0y * q0y * (-s000 + s100) + q0z * q0z * (-s000 + s100) +
                    qperpx * qperpy * s110 - qperpw * qperpz * s110 +
                    q0y * (q0x * (s010 - s110) + q0w * (s020 - s120)) +
                    qperpw * qperpy * s120 + qperpx * qperpz * s120 +
                    q0z * (-(q0w * s010) + q0x * s020 + q0w * s110 - q0x * s120)) *
                theta,
                2 * (qperpy * qperpy * s001 + qperpz * qperpz * s001 -
                    qperpx * qperpy * s011 + qperpw * qperpz * s011 -
                    qperpw * qperpy * s021 - qperpx * qperpz * s021 -
                    qperpy * qperpy * s101 - qperpz * qperpz * s101 +
                    q0y * q0y * (-s001 + s101) + q0z * q0z * (-s001 + s101) +
                    qperpx * qperpy * s111 - qperpw * qperpz * s111 +
                    q0y * (q0x * (s011 - s111) + q0w * (s021 - s121)) +
                    qperpw * qperpy * s121 + qperpx * qperpz * s121 +
                    q0z * (-(q0w * s011) + q0x * s021 + q0w * s111 - q0x * s121)) *
                theta,
                2 * (qperpy * qperpy * s002 + qperpz * qperpz * s002 -
                    qperpx * qperpy * s012 + qperpw * qperpz * s012 -
                    qperpw * qperpy * s022 - qperpx * qperpz * s022 -
                    qperpy * qperpy * s102 - qperpz * qperpz * s102 +
                    q0y * q0y * (-s002 + s102) + q0z * q0z * (-s002 + s102) +
                    qperpx * qperpy * s112 - qperpw * qperpz * s112 +
                    q0y * (q0x * (s012 - s112) + q0w * (s022 - s122)) +
                    qperpw * qperpy * s122 + qperpx * qperpz * s122 +
                    q0z * (-(q0w * s012) + q0x * s022 + q0w * s112 - q0x * s122)) *
                theta
            };

            c1[1] = DerivativeTerm{
                -t0y + t1y,
                -(qperpx * qperpy * s000) - qperpw * qperpz * s000 - s010 +
                q0z * q0z * s010 + qperpx * qperpx * s010 +
                qperpz * qperpz * s010 - q0y * q0z * s020 +
                qperpw * qperpx * s020 - qperpy * qperpz * s020 +
                qperpx * qperpy * s100 + qperpw * qperpz * s100 +
                q0w * q0z * (-s000 + s100) + q0x * q0x * (s010 - s110) + s110 -
                q0z * q0z * s110 - qperpx * qperpx * s110 -
                qperpz * qperpz * s110 +
                q0x * (q0y * (-s000 + s100) + q0w * (s020 - s120)) +
                q0y * q0z * s120 - qperpw * qperpx * s120 +
                qperpy * qperpz * s120,
                -(qperpx * qperpy * s001) - qperpw * qperpz * s001 - s011 +
                q0z * q0z * s011 + qperpx * qperpx * s011 +
                qperpz * qperpz * s011 - q0y * q0z * s021 +
                qperpw * qperpx * s021 - qperpy * qperpz * s021 +
                qperpx * qperpy * s101 + qperpw * qperpz * s101 +
                q0w * q0z * (-s001 + s101) + q0x * q0x * (s011 - s111) + s111 -
                q0z * q0z * s111 - qperpx * qperpx * s111 -
                qperpz * qperpz * s111 +
                q0x * (q0y * (-s001 + s101) + q0w * (s021 - s121)) +
                q0y * q0z * s121 - qperpw * qperpx * s121 +
                qperpy * qperpz * s121,
                -(qperpx * qperpy * s002) - qperpw * qperpz * s002 - s012 +
                q0z * q0z * s012 + qperpx * qperpx * s012 +
                qperpz * qperpz * s012 - q0y * q0z * s022 +
                qperpw * qperpx * s022 - qperpy * qperpz * s022 +
                qperpx * qperpy * s102 + qperpw * qperpz * s102 +
                q0w * q0z * (-s002 + s102) + q0x * q0x * (s012 - s112) + s112 -
                q0z * q0z * s112 - qperpx * qperpx * s112 -
                qperpz * qperpz * s112 +
                q0x * (q0y * (-s002 + s102) + q0w * (s022 - s122)) +
                q0y * q0z * s122 - qperpw * qperpx * s122 +
                qperpy * qperpz * s122
            };

            c2[1] = DerivativeTerm{
                0.,
                qperpx * qperpy * s000 + qperpw * qperpz * s000 + q0z * q0z * s010 -
                qperpx * qperpx * s010 - qperpz * qperpz * s010 -
                q0y * q0z * s020 - qperpw * qperpx * s020 +
                qperpy * qperpz * s020 - qperpx * qperpy * s100 -
                qperpw * qperpz * s100 + q0x * q0x * (s010 - s110) -
                q0z * q0z * s110 + qperpx * qperpx * s110 +
                qperpz * qperpz * s110 + q0y * q0z * s120 +
                qperpw * qperpx * s120 - qperpy * qperpz * s120 +
                2 * q0z * qperpw * s000 * theta +
                2 * q0y * qperpx * s000 * theta -
                4 * q0z * qperpz * s010 * theta +
                2 * q0z * qperpy * s020 * theta +
                2 * q0y * qperpz * s020 * theta +
                q0x * (q0w * s020 + q0y * (-s000 + s100) - q0w * s120 +
                    2 * qperpy * s000 * theta - 4 * qperpx * s010 * theta -
                    2 * qperpw * s020 * theta) +
                q0w * (-(q0z * s000) + q0z * s100 + 2 * qperpz * s000 * theta -
                    2 * qperpx * s020 * theta),
                qperpx * qperpy * s001 + qperpw * qperpz * s001 + q0z * q0z * s011 -
                qperpx * qperpx * s011 - qperpz * qperpz * s011 -
                q0y * q0z * s021 - qperpw * qperpx * s021 +
                qperpy * qperpz * s021 - qperpx * qperpy * s101 -
                qperpw * qperpz * s101 + q0x * q0x * (s011 - s111) -
                q0z * q0z * s111 + qperpx * qperpx * s111 +
                qperpz * qperpz * s111 + q0y * q0z * s121 +
                qperpw * qperpx * s121 - qperpy * qperpz * s121 +
                2 * q0z * qperpw * s001 * theta +
                2 * q0y * qperpx * s001 * theta -
                4 * q0z * qperpz * s011 * theta +
                2 * q0z * qperpy * s021 * theta +
                2 * q0y * qperpz * s021 * theta +
                q0x * (q0w * s021 + q0y * (-s001 + s101) - q0w * s121 +
                    2 * qperpy * s001 * theta - 4 * qperpx * s011 * theta -
                    2 * qperpw * s021 * theta) +
                q0w * (-(q0z * s001) + q0z * s101 + 2 * qperpz * s001 * theta -
                    2 * qperpx * s021 * theta),
                qperpx * qperpy * s002 + qperpw * qperpz * s002 + q0z * q0z * s012 -
                qperpx * qperpx * s012 - qperpz * qperpz * s012 -
                q0y * q0z * s022 - qperpw * qperpx * s022 +
                qperpy * qperpz * s022 - qperpx * qperpy * s102 -
                qperpw * qperpz * s102 + q0x * q0x * (s012 - s112) -
                q0z * q0z * s112 + qperpx * qperpx * s112 +
                qperpz * qperpz * s112 + q0y * q0z * s122 +
                qperpw * qperpx * s122 - qperpy * qperpz * s122 +
                2 * q0z * qperpw * s002 * theta +
                2 * q0y * qperpx * s002 * theta -
                4 * q0z * qperpz * s012 * theta +
                2 * q0z * qperpy * s022 * theta +
                2 * q0y * qperpz * s022 * theta +
                q0x * (q0w * s022 + q0y * (-s002 + s102) - q0w * s122 +
                    2 * qperpy * s002 * theta - 4 * qperpx * s012 * theta -
                    2 * qperpw * s022 * theta) +
                q0w * (-(q0z * s002) + q0z * s102 + 2 * qperpz * s002 * theta -
                    2 * qperpx * s022 * theta)
            };

            c3[1] = DerivativeTerm{
                0., 
                2 * (-(q0x * qperpy * s000) - q0w * qperpz * s000 +
                2 * q0x * qperpx * s010 + q0x * qperpw * s020 +
                q0w * qperpx * s020 + q0x * qperpy * s100 +
                q0w * qperpz * s100 - 2 * q0x * qperpx * s110 -
                q0x * qperpw * s120 - q0w * qperpx * s120 +
                q0z * (2 * qperpz * s010 - qperpy * s020 +
                    qperpw * (-s000 + s100) - 2 * qperpz * s110 +
                    qperpy * s120) +
                q0y * (-(qperpx * s000) - qperpz * s020 + qperpx * s100 +
                    qperpz * s120)) * theta,
                2 * (-(q0x * qperpy * s001) - q0w * qperpz * s001 +
                    2 * q0x * qperpx * s011 + q0x * qperpw * s021 +
                    q0w * qperpx * s021 + q0x * qperpy * s101 +
                    q0w * qperpz * s101 - 2 * q0x * qperpx * s111 -
                    q0x * qperpw * s121 - q0w * qperpx * s121 +
                    q0z * (2 * qperpz * s011 - qperpy * s021 +
                        qperpw * (-s001 + s101) - 2 * qperpz * s111 +
                        qperpy * s121) +
                    q0y * (-(qperpx * s001) - qperpz * s021 + qperpx * s101 +
                        qperpz * s121)) *
                theta,
                2 * (-(q0x * qperpy * s002) - q0w * qperpz * s002 +
                    2 * q0x * qperpx * s012 + q0x * qperpw * s022 +
                    q0w * qperpx * s022 + q0x * qperpy * s102 +
                    q0w * qperpz * s102 - 2 * q0x * qperpx * s112 -
                    q0x * qperpw * s122 - q0w * qperpx * s122 +
                    q0z * (2 * qperpz * s012 - qperpy * s022 +
                        qperpw * (-s002 + s102) - 2 * qperpz * s112 +
                        qperpy * s122) +
                    q0y * (-(qperpx * s002) - qperpz * s022 + qperpx * s102 +
                        qperpz * s122)) *
                theta
            };

            c4[1] = DerivativeTerm{
                0.,
                -(q0x * qperpy * s000) - q0w * qperpz * s000 +
                2 * q0x * qperpx * s010 + q0x * qperpw * s020 +
                q0w * qperpx * s020 + q0x * qperpy * s100 +
                q0w * qperpz * s100 - 2 * q0x * qperpx * s110 -
                q0x * qperpw * s120 - q0w * qperpx * s120 +
                2 * qperpx * qperpy * s000 * theta +
                2 * qperpw * qperpz * s000 * theta +
                2 * q0x * q0x * s010 * theta + 2 * q0z * q0z * s010 * theta -
                2 * qperpx * qperpx * s010 * theta -
                2 * qperpz * qperpz * s010 * theta +
                2 * q0w * q0x * s020 * theta -
                2 * qperpw * qperpx * s020 * theta +
                2 * qperpy * qperpz * s020 * theta +
                q0y * (-(qperpx * s000) - qperpz * s020 + qperpx * s100 +
                    qperpz * s120 - 2 * q0x * s000 * theta) +
                q0z * (2 * qperpz * s010 - qperpy * s020 +
                    qperpw * (-s000 + s100) - 2 * qperpz * s110 +
                    qperpy * s120 - 2 * q0w * s000 * theta -
                    2 * q0y * s020 * theta),
                -(q0x * qperpy * s001) - q0w * qperpz * s001 +
                2 * q0x * qperpx * s011 + q0x * qperpw * s021 +
                q0w * qperpx * s021 + q0x * qperpy * s101 +
                q0w * qperpz * s101 - 2 * q0x * qperpx * s111 -
                q0x * qperpw * s121 - q0w * qperpx * s121 +
                2 * qperpx * qperpy * s001 * theta +
                2 * qperpw * qperpz * s001 * theta +
                2 * q0x * q0x * s011 * theta + 2 * q0z * q0z * s011 * theta -
                2 * qperpx * qperpx * s011 * theta -
                2 * qperpz * qperpz * s011 * theta +
                2 * q0w * q0x * s021 * theta -
                2 * qperpw * qperpx * s021 * theta +
                2 * qperpy * qperpz * s021 * theta +
                q0y * (-(qperpx * s001) - qperpz * s021 + qperpx * s101 +
                    qperpz * s121 - 2 * q0x * s001 * theta) +
                q0z * (2 * qperpz * s011 - qperpy * s021 +
                    qperpw * (-s001 + s101) - 2 * qperpz * s111 +
                    qperpy * s121 - 2 * q0w * s001 * theta -
                    2 * q0y * s021 * theta),
                -(q0x * qperpy * s002) - q0w * qperpz * s002 +
                2 * q0x * qperpx * s012 + q0x * qperpw * s022 +
                q0w * qperpx * s022 + q0x * qperpy * s102 +
                q0w * qperpz * s102 - 2 * q0x * qperpx * s112 -
                q0x * qperpw * s122 - q0w * qperpx * s122 +
                2 * qperpx * qperpy * s002 * theta +
                2 * qperpw * qperpz * s002 * theta +
                2 * q0x * q0x * s012 * theta + 2 * q0z * q0z * s012 * theta -
                2 * qperpx * qperpx * s012 * theta -
                2 * qperpz * qperpz * s012 * theta +
                2 * q0w * q0x * s022 * theta -
                2 * qperpw * qperpx * s022 * theta +
                2 * qperpy * qperpz * s022 * theta +
                q0y * (-(qperpx * s002) - qperpz * s022 + qperpx * s102 +
                    qperpz * s122 - 2 * q0x * s002 * theta) +
                q0z * (2 * qperpz * s012 - qperpy * s022 +
                    qperpw * (-s002 + s102) - 2 * qperpz * s112 +
                    qperpy * s122 - 2 * q0w * s002 * theta -
                    2 * q0y * s022 * theta)
            };

            c5[1] = DerivativeTerm{
                0., -2 * (qperpx * qperpy * s000 + qperpw * qperpz * s000 +
                    q0z * q0z * s010 - qperpx * qperpx * s010 -
                    qperpz * qperpz * s010 - q0y * q0z * s020 -
                    qperpw * qperpx * s020 + qperpy * qperpz * s020 -
                    qperpx * qperpy * s100 - qperpw * qperpz * s100 +
                    q0w * q0z * (-s000 + s100) + q0x * q0x * (s010 - s110) -
                    q0z * q0z * s110 + qperpx * qperpx * s110 +
                    qperpz * qperpz * s110 +
                    q0x * (q0y * (-s000 + s100) + q0w * (s020 - s120)) +
                    q0y * q0z * s120 + qperpw * qperpx * s120 -
                    qperpy * qperpz * s120) *
                theta,
                -2 * (qperpx * qperpy * s001 + qperpw * qperpz * s001 +
                    q0z * q0z * s011 - qperpx * qperpx * s011 -
                    qperpz * qperpz * s011 - q0y * q0z * s021 -
                    qperpw * qperpx * s021 + qperpy * qperpz * s021 -
                    qperpx * qperpy * s101 - qperpw * qperpz * s101 +
                    q0w * q0z * (-s001 + s101) + q0x * q0x * (s011 - s111) -
                    q0z * q0z * s111 + qperpx * qperpx * s111 +
                    qperpz * qperpz * s111 +
                    q0x * (q0y * (-s001 + s101) + q0w * (s021 - s121)) +
                    q0y * q0z * s121 + qperpw * qperpx * s121 -
                    qperpy * qperpz * s121) *
                theta,
                -2 * (qperpx * qperpy * s002 + qperpw * qperpz * s002 +
                    q0z * q0z * s012 - qperpx * qperpx * s012 -
                    qperpz * qperpz * s012 - q0y * q0z * s022 -
                    qperpw * qperpx * s022 + qperpy * qperpz * s022 -
                    qperpx * qperpy * s102 - qperpw * qperpz * s102 +
                    q0w * q0z * (-s002 + s102) + q0x * q0x * (s012 - s112) -
                    q0z * q0z * s112 + qperpx * qperpx * s112 +
                    qperpz * qperpz * s112 +
                    q0x * (q0y * (-s002 + s102) + q0w * (s022 - s122)) +
                    q0y * q0z * s122 + qperpw * qperpx * s122 -
                    qperpy * qperpz * s122) *
                theta
            };

            c1[2] = DerivativeTerm{
                -t0z + t1z, (qperpw * qperpy * s000 - qperpx * qperpz * s000 -
                    q0y * q0z * s010 - qperpw * qperpx * s010 -
                    qperpy * qperpz * s010 - s020 + q0y * q0y * s020 +
                    qperpx * qperpx * s020 + qperpy * qperpy * s020 -
                    qperpw * qperpy * s100 + qperpx * qperpz * s100 +
                    q0x * q0z * (-s000 + s100) + q0y * q0z * s110 +
                    qperpw * qperpx * s110 + qperpy * qperpz * s110 +
                    q0w * (q0y * (s000 - s100) + q0x * (-s010 + s110)) +
                    q0x * q0x * (s020 - s120) + s120 - q0y * q0y * s120 -
                    qperpx * qperpx * s120 - qperpy * qperpy * s120),
                    (qperpw * qperpy * s001 - qperpx * qperpz * s001 -
                        q0y * q0z * s011 - qperpw * qperpx * s011 -
                        qperpy * qperpz * s011 - s021 + q0y * q0y * s021 +
                        qperpx * qperpx * s021 + qperpy * qperpy * s021 -
                        qperpw * qperpy * s101 + qperpx * qperpz * s101 +
                        q0x * q0z * (-s001 + s101) + q0y * q0z * s111 +
                        qperpw * qperpx * s111 + qperpy * qperpz * s111 +
                        q0w * (q0y * (s001 - s101) + q0x * (-s011 + s111)) +
                        q0x * q0x * (s021 - s121) + s121 - q0y * q0y * s121 -
                        qperpx * qperpx * s121 - qperpy * qperpy * s121),
                        (qperpw * qperpy * s002 - qperpx * qperpz * s002 -
                            q0y * q0z * s012 - qperpw * qperpx * s012 -
                            qperpy * qperpz * s012 - s022 + q0y * q0y * s022 +
                            qperpx * qperpx * s022 + qperpy * qperpy * s022 -
                            qperpw * qperpy * s102 + qperpx * qperpz * s102 +
                            q0x * q0z * (-s002 + s102) + q0y * q0z * s112 +
                            qperpw * qperpx * s112 + qperpy * qperpz * s112 +
                            q0w * (q0y * (s002 - s102) + q0x * (-s012 + s112)) +
                            q0x * q0x * (s022 - s122) + s122 - q0y * q0y * s122 -
                            qperpx * qperpx * s122 - qperpy * qperpy * s122)
            };

            c2[2] = DerivativeTerm{
                0.,
                (q0w * q0y * s000 - q0x * q0z * s000 - qperpw * qperpy * s000 +
                    qperpx * qperpz * s000 - q0w * q0x * s010 - q0y * q0z * s010 +
                    qperpw * qperpx * s010 + qperpy * qperpz * s010 +
                    q0x * q0x * s020 + q0y * q0y * s020 - qperpx * qperpx * s020 -
                    qperpy * qperpy * s020 - q0w * q0y * s100 + q0x * q0z * s100 +
                    qperpw * qperpy * s100 - qperpx * qperpz * s100 +
                    q0w * q0x * s110 + q0y * q0z * s110 - qperpw * qperpx * s110 -
                    qperpy * qperpz * s110 - q0x * q0x * s120 - q0y * q0y * s120 +
                    qperpx * qperpx * s120 + qperpy * qperpy * s120 -
                    2 * q0y * qperpw * s000 * theta + 2 * q0z * qperpx * s000 * theta -
                    2 * q0w * qperpy * s000 * theta + 2 * q0x * qperpz * s000 * theta +
                    2 * q0x * qperpw * s010 * theta + 2 * q0w * qperpx * s010 * theta +
                    2 * q0z * qperpy * s010 * theta + 2 * q0y * qperpz * s010 * theta -
                    4 * q0x * qperpx * s020 * theta - 4 * q0y * qperpy * s020 * theta),
                    (q0w * q0y * s001 - q0x * q0z * s001 - qperpw * qperpy * s001 +
                        qperpx * qperpz * s001 - q0w * q0x * s011 - q0y * q0z * s011 +
                        qperpw * qperpx * s011 + qperpy * qperpz * s011 +
                        q0x * q0x * s021 + q0y * q0y * s021 - qperpx * qperpx * s021 -
                        qperpy * qperpy * s021 - q0w * q0y * s101 + q0x * q0z * s101 +
                        qperpw * qperpy * s101 - qperpx * qperpz * s101 +
                        q0w * q0x * s111 + q0y * q0z * s111 - qperpw * qperpx * s111 -
                        qperpy * qperpz * s111 - q0x * q0x * s121 - q0y * q0y * s121 +
                        qperpx * qperpx * s121 + qperpy * qperpy * s121 -
                        2 * q0y * qperpw * s001 * theta + 2 * q0z * qperpx * s001 * theta -
                        2 * q0w * qperpy * s001 * theta + 2 * q0x * qperpz * s001 * theta +
                        2 * q0x * qperpw * s011 * theta + 2 * q0w * qperpx * s011 * theta +
                        2 * q0z * qperpy * s011 * theta + 2 * q0y * qperpz * s011 * theta -
                        4 * q0x * qperpx * s021 * theta - 4 * q0y * qperpy * s021 * theta),
                        (q0w * q0y * s002 - q0x * q0z * s002 - qperpw * qperpy * s002 +
                            qperpx * qperpz * s002 - q0w * q0x * s012 - q0y * q0z * s012 +
                            qperpw * qperpx * s012 + qperpy * qperpz * s012 +
                            q0x * q0x * s022 + q0y * q0y * s022 - qperpx * qperpx * s022 -
                            qperpy * qperpy * s022 - q0w * q0y * s102 + q0x * q0z * s102 +
                            qperpw * qperpy * s102 - qperpx * qperpz * s102 +
                            q0w * q0x * s112 + q0y * q0z * s112 - qperpw * qperpx * s112 -
                            qperpy * qperpz * s112 - q0x * q0x * s122 - q0y * q0y * s122 +
                            qperpx * qperpx * s122 + qperpy * qperpy * s122 -
                            2 * q0y * qperpw * s002 * theta + 2 * q0z * qperpx * s002 * theta -
                            2 * q0w * qperpy * s002 * theta + 2 * q0x * qperpz * s002 * theta +
                            2 * q0x * qperpw * s012 * theta + 2 * q0w * qperpx * s012 * theta +
                            2 * q0z * qperpy * s012 * theta + 2 * q0y * qperpz * s012 * theta -
                            4 * q0x * qperpx * s022 * theta -
                            4 * q0y * qperpy * s022 * theta)
            };

            c3[2] = DerivativeTerm{
                0., -2 * (-(q0w * qperpy * s000) + q0x * qperpz * s000 +
                    q0x * qperpw * s010 + q0w * qperpx * s010 -
                    2 * q0x * qperpx * s020 + q0w * qperpy * s100 -
                    q0x * qperpz * s100 - q0x * qperpw * s110 -
                    q0w * qperpx * s110 +
                    q0z * (qperpx * s000 + qperpy * s010 - qperpx * s100 -
                        qperpy * s110) +
                    2 * q0x * qperpx * s120 +
                    q0y * (qperpz * s010 - 2 * qperpy * s020 +
                        qperpw * (-s000 + s100) - qperpz * s110 +
                        2 * qperpy * s120)) *
                theta,
                -2 * (-(q0w * qperpy * s001) + q0x * qperpz * s001 +
                    q0x * qperpw * s011 + q0w * qperpx * s011 -
                    2 * q0x * qperpx * s021 + q0w * qperpy * s101 -
                    q0x * qperpz * s101 - q0x * qperpw * s111 -
                    q0w * qperpx * s111 +
                    q0z * (qperpx * s001 + qperpy * s011 - qperpx * s101 -
                        qperpy * s111) +
                    2 * q0x * qperpx * s121 +
                    q0y * (qperpz * s011 - 2 * qperpy * s021 +
                        qperpw * (-s001 + s101) - qperpz * s111 +
                        2 * qperpy * s121)) *
                theta,
                -2 * (-(q0w * qperpy * s002) + q0x * qperpz * s002 +
                    q0x * qperpw * s012 + q0w * qperpx * s012 -
                    2 * q0x * qperpx * s022 + q0w * qperpy * s102 -
                    q0x * qperpz * s102 - q0x * qperpw * s112 -
                    q0w * qperpx * s112 +
                    q0z * (qperpx * s002 + qperpy * s012 - qperpx * s102 -
                        qperpy * s112) +
                    2 * q0x * qperpx * s122 +
                    q0y * (qperpz * s012 - 2 * qperpy * s022 +
                        qperpw * (-s002 + s102) - qperpz * s112 +
                        2 * qperpy * s122)) *
                theta
            };

            c4[2] = DerivativeTerm{
                0.,
                q0w * qperpy * s000 - q0x * qperpz * s000 - q0x * qperpw * s010 -
                q0w * qperpx * s010 + 2 * q0x * qperpx * s020 -
                q0w * qperpy * s100 + q0x * qperpz * s100 +
                q0x * qperpw * s110 + q0w * qperpx * s110 -
                2 * q0x * qperpx * s120 - 2 * qperpw * qperpy * s000 * theta +
                2 * qperpx * qperpz * s000 * theta -
                2 * q0w * q0x * s010 * theta +
                2 * qperpw * qperpx * s010 * theta +
                2 * qperpy * qperpz * s010 * theta +
                2 * q0x * q0x * s020 * theta + 2 * q0y * q0y * s020 * theta -
                2 * qperpx * qperpx * s020 * theta -
                2 * qperpy * qperpy * s020 * theta +
                q0z * (-(qperpx * s000) - qperpy * s010 + qperpx * s100 +
                    qperpy * s110 - 2 * q0x * s000 * theta) +
                q0y * (-(qperpz * s010) + 2 * qperpy * s020 +
                    qperpw * (s000 - s100) + qperpz * s110 -
                    2 * qperpy * s120 + 2 * q0w * s000 * theta -
                    2 * q0z * s010 * theta),
                q0w * qperpy * s001 - q0x * qperpz * s001 - q0x * qperpw * s011 -
                q0w * qperpx * s011 + 2 * q0x * qperpx * s021 -
                q0w * qperpy * s101 + q0x * qperpz * s101 +
                q0x * qperpw * s111 + q0w * qperpx * s111 -
                2 * q0x * qperpx * s121 - 2 * qperpw * qperpy * s001 * theta +
                2 * qperpx * qperpz * s001 * theta -
                2 * q0w * q0x * s011 * theta +
                2 * qperpw * qperpx * s011 * theta +
                2 * qperpy * qperpz * s011 * theta +
                2 * q0x * q0x * s021 * theta + 2 * q0y * q0y * s021 * theta -
                2 * qperpx * qperpx * s021 * theta -
                2 * qperpy * qperpy * s021 * theta +
                q0z * (-(qperpx * s001) - qperpy * s011 + qperpx * s101 +
                    qperpy * s111 - 2 * q0x * s001 * theta) +
                q0y * (-(qperpz * s011) + 2 * qperpy * s021 +
                    qperpw * (s001 - s101) + qperpz * s111 -
                    2 * qperpy * s121 + 2 * q0w * s001 * theta -
                    2 * q0z * s011 * theta),
                q0w * qperpy * s002 - q0x * qperpz * s002 - q0x * qperpw * s012 -
                q0w * qperpx * s012 + 2 * q0x * qperpx * s022 -
                q0w * qperpy * s102 + q0x * qperpz * s102 +
                q0x * qperpw * s112 + q0w * qperpx * s112 -
                2 * q0x * qperpx * s122 - 2 * qperpw * qperpy * s002 * theta +
                2 * qperpx * qperpz * s002 * theta -
                2 * q0w * q0x * s012 * theta +
                2 * qperpw * qperpx * s012 * theta +
                2 * qperpy * qperpz * s012 * theta +
                2 * q0x * q0x * s022 * theta + 2 * q0y * q0y * s022 * theta -
                2 * qperpx * qperpx * s022 * theta -
                2 * qperpy * qperpy * s022 * theta +
                q0z * (-(qperpx * s002) - qperpy * s012 + qperpx * s102 +
                    qperpy * s112 - 2 * q0x * s002 * theta) +
                q0y * (-(qperpz * s012) + 2 * qperpy * s022 +
                    qperpw * (s002 - s102) + qperpz * s112 -
                    2 * qperpy * s122 + 2 * q0w * s002 * theta -
                    2 * q0z * s012 * theta)
            };

            c5[2] = DerivativeTerm{
                0., 2 * (qperpw * qperpy * s000 - qperpx * qperpz * s000 +
                    q0y * q0z * s010 - qperpw * qperpx * s010 -
                    qperpy * qperpz * s010 - q0y * q0y * s020 +
                    qperpx * qperpx * s020 + qperpy * qperpy * s020 +
                    q0x * q0z * (s000 - s100) - qperpw * qperpy * s100 +
                    qperpx * qperpz * s100 +
                    q0w * (q0y * (-s000 + s100) + q0x * (s010 - s110)) -
                    q0y * q0z * s110 + qperpw * qperpx * s110 +
                    qperpy * qperpz * s110 + q0y * q0y * s120 -
                    qperpx * qperpx * s120 - qperpy * qperpy * s120 +
                    q0x * q0x * (-s020 + s120)) *
                theta,
                2 * (qperpw * qperpy * s001 - qperpx * qperpz * s001 +
                    q0y * q0z * s011 - qperpw * qperpx * s011 -
                    qperpy * qperpz * s011 - q0y * q0y * s021 +
                    qperpx * qperpx * s021 + qperpy * qperpy * s021 +
                    q0x * q0z * (s001 - s101) - qperpw * qperpy * s101 +
                    qperpx * qperpz * s101 +
                    q0w * (q0y * (-s001 + s101) + q0x * (s011 - s111)) -
                    q0y * q0z * s111 + qperpw * qperpx * s111 +
                    qperpy * qperpz * s111 + q0y * q0y * s121 -
                    qperpx * qperpx * s121 - qperpy * qperpy * s121 +
                    q0x * q0x * (-s021 + s121)) *
                theta,
                2 * (qperpw * qperpy * s002 - qperpx * qperpz * s002 +
                    q0y * q0z * s012 - qperpw * qperpx * s012 -
                    qperpy * qperpz * s012 - q0y * q0y * s022 +
                    qperpx * qperpx * s022 + qperpy * qperpy * s022 +
                    q0x * q0z * (s002 - s102) - qperpw * qperpy * s102 +
                    qperpx * qperpz * s102 +
                    q0w * (q0y * (-s002 + s102) + q0x * (s012 - s112)) -
                    q0y * q0z * s112 + qperpw * qperpx * s112 +
                    qperpy * qperpz * s112 + q0y * q0y * s122 -
                    qperpx * qperpx * s122 - qperpy * qperpy * s122 +
                    q0x * q0x * (-s022 + s122)) *
                theta
            };
            // clang-format on
        }
    }

    Transformation AnimatedTransformation::interpolate(const Float time) const {
        if (!actuallyAnimated || time <= startTime) {
            return *startTransform;
        }
        else if (time >= endTime) {
            return *endTransform;
        }

        const auto dt = (time - startTime) / (endTime - startTime);
        const auto T = lerp(dt, startTRS.T, endTRS.T);
        const auto R = slerp(dt, startTRS.R, endTRS.R);

        auto S = Matrix4x4{};
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; j < 3; ++j) {
                S.m[i][j] = lerp(dt, startTRS.S.m[i][j], endTRS.S.m[i][j]);
            }
        }

        return translation(T) * R.toTransformation() * Transformation{S};
    }

    Bounds3f AnimatedTransformation::motionBounds(const Bounds3f& b) const {
        if (!actuallyAnimated) {
            return (*startTransform)(b);
        }
        else if (!hasRotation) {
            return unionOf((*startTransform)(b), (*endTransform)(b));
        }
        else {
            Bounds3f bounds;
            for (std::size_t i = 0; i < 8; ++i) {
                const auto cornerBounds = pointMotionBounds(b.corner(i));
                bounds = unionOf(bounds, cornerBounds);
            }
            return bounds;
        }
    }

    Bounds3f AnimatedTransformation::pointMotionBounds(const Point3f& p) const {
        if (!actuallyAnimated) {
            return Bounds3f{(*startTransform)(p)};
        }

        const auto cosTheta = dot(startTRS.R, endTRS.R);
        const auto theta = std::acos(clamp(cosTheta, -1.f, 1.f));
        const auto tInterval = Interval{0.f, 1.f};

        auto bounds = Bounds3f{(*startTransform)(p), (*endTransform)(p)};
        for (std::size_t c = 0; c < 3; ++c) {
            const auto cs =
                Coefficients{c1[c](p), c2[c](p), c3[c](p), c4[c](p), c5[c](p)};

            Float zeros[8];
            unsigned zerosCount = 0;
            intervalFindZeros(cs, theta, tInterval, zeros, zerosCount);

            assert(zerosCount <= std::extent_v<decltype(zeros)>);

            for (const auto tZero : zeros) {
                const auto t = lerp(tZero, startTime, endTime);
                const auto tPoint = (*this)(t, p);
                bounds = unionOf(bounds, tPoint);
            }
        }
        return bounds;
    }

    void AnimatedTransformation::intervalFindZeros(const Coefficients& cs,
                                                   const Float theta,
                                                   const Interval& tInterval,
                                                   Float zeros[8],
                                                   unsigned& zerosCount,
                                                   int depth) {
        const auto range = Interval{cs.c1} +
                           (Interval{cs.c2} + Interval{cs.c3} * tInterval) *
                               cos(Interval{2.f * theta} * tInterval) +
                           (Interval{cs.c4} + Interval{cs.c5} * tInterval) *
                               sin(Interval{2.f * theta} * tInterval);
        if (range.low() > 0.f || range.high() < 0.f ||
            range.low() == range.high()) {
            return;
        }

        if (depth > 0) {
            const auto mid = (tInterval.low() + tInterval.high()) * 0.5f;
            intervalFindZeros(cs,
                              theta,
                              tInterval.withHigh(mid),
                              zeros,
                              zerosCount,
                              depth - 1);
            intervalFindZeros(cs,
                              theta,
                              tInterval.withLow(mid),
                              zeros,
                              zerosCount,
                              depth - 1);
        }
        else {
            auto tNewton = (tInterval.low() + tInterval.high()) * 0.5f;
            for (auto i = 0; i < 4; ++i) {
                const auto fNewton =
                    cs.c1 +
                    (cs.c2 + cs.c3 * tNewton) *
                        std::cos(2.f * theta * tNewton) +
                    (cs.c4 + cs.c5 * tNewton) * std::sin(2.f * theta * tNewton);
                const auto fPrimeNewton =
                    (cs.c3 + 2.f * (cs.c4 + cs.c5 * tNewton) * theta) *
                        std::cos(2.f * tNewton * theta) +
                    (cs.c5 - 2.f * (cs.c2 + cs.c3 * tNewton) * theta) *
                        std::sin(2.f * tNewton * theta);

                if (fNewton == 0.f || fPrimeNewton == 0.f) {
                    break;
                }

                tNewton = tNewton - fNewton / fPrimeNewton;
            }

            if (tNewton >= tInterval.low() - 1e-3f &&
                tNewton < tInterval.high() + 1e-3f) {
                zeros[zerosCount++] = tNewton;
            }
        }
    }

    Float AnimatedTransformation::DerivativeTerm::operator()(
        const Point3f& p) const noexcept {
        return kc + kx * p.x + ky * p.y + kz * p.z;
    }

    // will be instantiated only in this translation unit
    // so it is fine to define it here
    template <typename T>
    auto AnimatedTransformation::transform(const Float time, const T& x) const {
        if (!actuallyAnimated || time <= startTime) {
            return (*startTransform)(x);
        }
        else if (time >= endTime) {
            return (*endTransform)(x);
        }
        else {
            const auto transformation = this->interpolate(time);
            return transformation(x);
        }
    }

    Ray AnimatedTransformation::operator()(const Ray& ray) const {
        return transform(ray.time, ray);
    }

    RayDifferential
    AnimatedTransformation::operator()(const RayDifferential& ray) const {
        return transform(ray.time, ray);
    }

    Point3f AnimatedTransformation::operator()(const Float time,
                                               const Point3f& p) const {
        return transform(time, p);
    }

    Vector3f AnimatedTransformation::operator()(const Float time,
                                                const Vector3f& v) const {
        return transform(time, v);
    }
} // namespace idragnev::pbrt