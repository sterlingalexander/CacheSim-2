#include <iostream>
#include <vector>
#include "directory.h"
#include "entry.h"

extern int DEBUG;
extern int DIRECTORY_SIZE;

directory::directory(int size) {
    
   position = new entry[size];
   for (int i = 0; i < size; i++) {
      // position[i] = new entry;
      //position[i] = new entry();
      position[i].clearAll();
      if (DEBUG) cout << "Constructed position[" << i << "]\n";
      if (DEBUG) cout << position[i];
   }
}

int findTagPos(ulong addr)  {
   
   for (int i = 0; i < DIRECTORY_SIZE; i++)  {
      if (position[i].tag == addr)
         return i;
   }
   return -1;
}
 
int findUnownedPos()  {
   
   for (int i = 0; i < DIRECTORY_SIZE; i++)  {
      if (position[i].state == UNOWNED)
         return i;
   }
   return -1;
}
 
ostream &operator<<(ostream &stream, directory obj)  { 
   
   for (int i = 0; i < DIRECTORY_SIZE; ++i)  {
      stream << obj.position[i].tag << ", "; 
      stream << obj.position[i].state << ", "; 
      stream << obj.position[i].processor[0] << ", " << obj.position[i].processor[1] << ", ";
      stream << obj.position[i].processor[2] << ", " << obj.position[i].processor[3] << ", ";
      stream << obj.position[i].dirty << "\n"; 
   }
   return stream;  // return the stream 
}
