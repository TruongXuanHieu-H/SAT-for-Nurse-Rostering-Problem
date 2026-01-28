#ifndef ENCODE_TYPE_H
#define ENCODE_TYPE_H

#include <string>

enum class EncodeType
{
    BDD,        // Binary decision diagram
    Card,       // Cardinality
    Pairwise,   // Reduced pairwise
    SCL,        // SCL 
    Seq,        // Sequential counter

};

inline std::string encode_type_to_string(EncodeType type) {
    switch (type) {
        case EncodeType::BDD:      
            return "BDD";
        case EncodeType::Card:     
            return "Card";
        case EncodeType::Pairwise: 
            return "Pairwise";
        case EncodeType::SCL:      
            return "SCL";
        case EncodeType::Seq:      
            return "Seq";
        default:                   
            return "UNKNOWN";
    }
}

#endif // ENCODE_TYPE_H