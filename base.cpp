#include "base.h"

void handleError(const std::string& errorMessage) {
    std::cerr << "Error: " << errorMessage << std::endl;
    std::exit(EXIT_FAILURE); 
}

std::string extractTrueFunName(const std::string& input) {
    std::string result;
    size_t endPos = input.find('(');
    if (endPos != std::string::npos) {
        std::string beforeParen = input.substr(0, endPos);
        size_t colonPos = beforeParen.find_last_not_of(':');
        if (colonPos != std::string::npos) {
            beforeParen = beforeParen.substr(0, colonPos+1);
        }
        size_t hashPos = beforeParen.find_last_of('#');
        std::string result;
        if (hashPos != std::string::npos) {
            result = beforeParen.substr(hashPos + 1);
        } else {
            result = beforeParen;
        }
        return result;
    }else{
        return input;
    }
}