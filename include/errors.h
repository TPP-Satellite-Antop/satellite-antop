#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
#include <string>

namespace Errors {
	inline const auto RESOLUTION_NOT_SUPPORTED = std::out_of_range("resolution out of range");
	inline auto fetchNeighbors(const H3Error status, const H3Index idx) {
		return std::runtime_error("failed to find neighbors for " + std::to_string(idx) + " (code " + std::to_string(status) + ").");
	}

	inline auto localCoordIJK(const H3Error status, const H3Index origin, const H3Index idx) {
		return std::runtime_error("failed to compute local IJK coordinates from " + std::to_string(origin) + " to " + std::to_string(idx) + " (code " + std::to_string(status) + ").");
	}
}

#endif // ERRORS_H