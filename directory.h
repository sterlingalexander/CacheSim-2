#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <cmath>
#include <iostream>
#include <vector>
#include "entry.h"

using namespace std;

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned int uint;

class directory  {
   public:

      directory( int );          // constructor
      ~directory()  {  }         // destructor
      friend ostream& operator<<(ostream &stream, directory obj);    // useful for debugging small traces
      
      int findTagPos(ulong addr);      // returns position of found tag
      int findUnownedPos();            // returns position of first unowned block
      
      entry *position;                 // pointer to array of directory entries.
   
};

#endif