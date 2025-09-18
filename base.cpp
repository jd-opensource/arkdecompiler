#include "base.h"

void handleError(const std::string& errorMessage) {
    std::cerr << "Error: " << errorMessage << std::endl;
    std::exit(EXIT_FAILURE); 
}

std::string extractTrueFunName(const std::string& input) {
    std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    std::cout << input << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------------------------------------------" << std::endl; 

    std::string result;
    size_t endPos = input.find('(');
    if (endPos != std::string::npos) {
        std::string beforeParen = input.substr(0, endPos);
        std::cout << "0: " << beforeParen << std::endl;
        size_t colonPos = beforeParen.find_last_not_of(':');
        if (colonPos != std::string::npos) {
            beforeParen = beforeParen.substr(0, colonPos+1);
        }
        std::cout << "1: " << beforeParen << std::endl;
        size_t hashPos = beforeParen.find_last_of('#');
        std::string result;
        if (hashPos != std::string::npos) {
            result = beforeParen.substr(hashPos + 1);
        } else {
            result = beforeParen;
        }
        
        std::cout << result << std::endl;
        std::cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" << std::endl;
        return result;
    }else{
        return input;
    }
}