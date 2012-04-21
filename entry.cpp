#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "entry.h"

using namespace std;

// Nothing to see here but an overloaded operator.  Most functions are inline in header

ostream &operator<<(ostream &stream, entry obj) {

    stream << obj.tag << ", ";
    stream << obj.state << ", ";
    stream << obj.processor[0] << ", " << obj.processor[1] << ", ";
    stream << obj.processor[2] << ", " << obj.processor[3] << "\n";

    return stream; // return the stream 
}