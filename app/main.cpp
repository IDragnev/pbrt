#include <iostream>

#include "Ray.hpp"
#include "Bounds2.hpp"
#include "Matrix4x4.hpp"

namespace pbrt = idragnev::pbrt;

int main() {
	const auto matrix = pbrt::Matrix4x4{};
	const auto inversed = pbrt::inverse(matrix);
	const auto transposed = pbrt::transpose(matrix);

	assert(matrix == inversed);
	assert(matrix == transposed);

	return 0;
}