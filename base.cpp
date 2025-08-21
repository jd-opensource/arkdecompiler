#include "base.h"

void handleError(const std::string& errorMessage) {
    std::cerr << "Error: " << errorMessage << std::endl;
    std::exit(EXIT_FAILURE); 
}
