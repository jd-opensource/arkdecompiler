#include "base.h"

void handleError(const std::string& errorMessage) {
    std::cerr << "Error: " << errorMessage << std::endl;
    std::exit(EXIT_FAILURE); 
}

std::string RemoveArgumentsOfFunc(const std::string& input) {
    std::string result;
    size_t endPos = input.find('(');
    if (endPos != std::string::npos) {
        std::string beforeParen = input.substr(0, endPos);
        size_t colonPos = beforeParen.find_last_not_of(':');
        if (colonPos != std::string::npos) {
            beforeParen = beforeParen.substr(0, colonPos+1);
            return beforeParen;
        }
    }

    return input;
    
}

bool IsArray(const panda::panda_file::LiteralTag &tag)
{
    switch (tag) {
        case panda::panda_file::LiteralTag::ARRAY_U1:
        case panda::panda_file::LiteralTag::ARRAY_U8:
        case panda::panda_file::LiteralTag::ARRAY_I8:
        case panda::panda_file::LiteralTag::ARRAY_U16:
        case panda::panda_file::LiteralTag::ARRAY_I16:
        case panda::panda_file::LiteralTag::ARRAY_U32:
        case panda::panda_file::LiteralTag::ARRAY_I32:
        case panda::panda_file::LiteralTag::ARRAY_U64:
        case panda::panda_file::LiteralTag::ARRAY_I64:
        case panda::panda_file::LiteralTag::ARRAY_F32:
        case panda::panda_file::LiteralTag::ARRAY_F64:
        case panda::panda_file::LiteralTag::ARRAY_STRING:
            return true;
        default:
            return false;
    }
}

std::optional<std::vector<std::string>> getLiteralArrayByOffset(panda::pandasm::Program* program, uint32_t offset){
    std::vector<std::string> res;
    std::stringstream hexStream;
    hexStream << "0x" << std::hex << offset;
    std::string offsetString = hexStream.str();
    
    for (const auto &[key, lit_array] :  program->literalarray_table) {
        const auto &tag = lit_array.literals_[0].tag_;
                
        if (key.find(offsetString) == std::string::npos || IsArray(tag)) {
            continue;
        }
        
        for (size_t i = 0; i < lit_array.literals_.size(); i++) {
            const auto &tag = lit_array.literals_[i].tag_;
            const auto &val = lit_array.literals_[i].value_;
            if(tag == panda::panda_file::LiteralTag::METHOD || tag == panda::panda_file::LiteralTag::GETTER || 
                tag == panda::panda_file::LiteralTag::SETTER || tag == panda::panda_file::LiteralTag::GENERATORMETHOD){
                res.push_back(std::get<std::string>(val));
            }
        }
        return res;
    }

    return std::nullopt;
}
