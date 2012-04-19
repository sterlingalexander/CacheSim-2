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

      directory( int );
      ~directory()  {  }
      friend ostream& operator<<(ostream &stream, directory obj);
      
      int findTagPos(ulong addr);
      int findUnownedPos();
      
      entry *position;
   
};

#endif