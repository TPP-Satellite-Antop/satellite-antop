#ifndef ERRORS_H
#define ERRORS_H

#include <string>

namespace Errors {
    inline const std::string COORD_CONVERTING_ERROR = "Error converting coordinate to H3 index.";
    inline std::string getNeighborsSearchError(int idx) {
    	return std::format("Error finding index {} neighbors.", idx);
	}
}

#endif // ERRORS_H
