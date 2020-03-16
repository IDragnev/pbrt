#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "Vector3.hpp"
#include "Normal3.hpp"

namespace pbrt = idragnev::pbrt;

TEST_CASE("coordinateSystem returns orthonormal basis when given normalized vector")
{
	const auto [u, v, w] = pbrt::coordinateSystem(pbrt::Vector3f{1.f, 0.f, 0.f});

	CHECK(u.length() == 1.f);
	CHECK(v.length() == 1.f);
	CHECK(w.length() == 1.f);

	CHECK(dot(u, v) == 0.f);
	CHECK(dot(u, w) == 0.f);
	CHECK(dot(w, v) == 0.f);
}

TEST_CASE("faceforward flips a normal so that it lies in the same hemisphere"
	      "as a vector")
{
	const auto v = pbrt::Vector3f{1.f, 0.f, 0.f};

	SUBCASE("same hemisphere")
	{
		const auto n = pbrt::Normal3f{2.f, 0.f, 0.f};

		const auto flipped = pbrt::faceforward(n, v);

		CHECK(flipped == n);
	}
	
	SUBCASE("opposite hemisphere")
	{
		const auto n = pbrt::Normal3f{-2.f, 0.f, 0.f};

		const auto flipped = pbrt::faceforward(n, v);

		CHECK(flipped == -n);
	}
}