#include <iostream>
#include "PnlVectToJson.hpp"

std::ostream&
operator<<(std::ostream& stm, const PnlVect* const vect)
{
    int length = vect->size;
    if (length == 0) {
        stm << "[]";
        return stm;
    }
    stm << '[';
    for (int i = 0; i < length - 1; i++) {
        stm << GET(vect, i) << ", ";
    }
    stm << GET(vect, length - 1) << "]";
    return stm;
}