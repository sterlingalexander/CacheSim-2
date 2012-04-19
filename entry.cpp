#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "entry.h"

using namespace std;


ostream &operator<<(ostream &stream, entry obj)  { 
      
      stream << obj.tag << ", "; 
      stream << obj.state << ", "; 
      stream << obj.processor[0] << ", " << obj.processor[1] << ", ";
      stream << obj.processor[2] << ", " << obj.processor[3] << ", ";
      stream << obj.dirty << "\n"; 
      
   return stream;  // return the stream 
}