#include <iostream>
#include <vector>
#include "directory.h"
#include "entry.h"

extern int DEBUG;
extern int DIRECTORY_SIZE;

// Constructor
directory::directory(int size) {
    
   position = new entry[size];         // create array of entries
   for (int i = 0; i < size; i++) {    
      position[i].clearAll();          // initalize all directory positions
      if (DEBUG) cout << "Constructed position[" << i << "]\n";      // debug output for small traces
      if (DEBUG) cout << position[i];                                // debug output for small traces
   }
}

// returns index of the found tag, returns -1 if tag is not in directory
int directory::findTagPos(ulong tag)  {
   
   for (int i = 0; i < DIRECTORY_SIZE; i++)  {  // linear traversal search...quick but inefficient
      if (position[i].tag == tag && !position[i].isU())
         return i;
   }
   return -1;
}
 
 // returns index of first unowned position, returns -1 if directory is full
int directory::findUnownedPos()  {
   
   for (int i = 0; i < DIRECTORY_SIZE; i++)  {  // linear traversal search...quick but inefficient
      if (position[i].isU())
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
