#include <iostream>

#include "core/Ray.hpp"
#include "core/Bounds2.hpp"
#include "core/Matrix4x4.hpp"
#include "core/Quaternion.hpp"

namespace pbrt = idragnev::pbrt;

int main() {
	const auto q = pbrt::Quaternion{};

	const auto matrix = pbrt::Matrix4x4{};
	const auto inversed = pbrt::inverse(matrix);
	const auto transposed = pbrt::transpose(matrix);

	assert(matrix == inversed);
	assert(matrix == transposed);

	return 0;
}