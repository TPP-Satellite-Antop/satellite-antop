#ifndef ERRORS_H
#define ERRORS_H

#include <string>

namespace Errors {
	inline const std::string COORD_CONVERTING_ERROR = "Error converting coordinate to H3 index.";
	inline const std::string RESOLUTION_NOT_SUPPORTED = "getOriginForResolution: resolution out of range";
	inline std::string getNeighborsSearchError(H3Index idx) {
		return "Error finding index " + std::to_string(idx) + " neighbors.";
	}
}

#endif // ERRORS_H