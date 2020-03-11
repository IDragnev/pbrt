#include <iostream>

#include "Ray.hpp"
#include "Bounds2.hpp"

namespace pbrt = idragnev::pbrt;

int main() {
	const auto bounds = pbrt::Bounds2<int>{ {0, 0}, {3, 1} };
	for (const auto& p : bounds) {
		std::cout << p.x << ' ';
	}

	return 0;
}