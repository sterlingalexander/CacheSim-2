#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

#include "directory.h"
#include "entry.h"

int NODES = 4;
int DIRECTORY_SIZE = 0;
int DEBUG = 0;

int main(int argc, char *argv[]) {

    if (argc > 1)
        DEBUG = atoi(argv[1]);

    int size = 10;
    DIRECTORY_SIZE = size;

    directory dir(size);
    dir.position[5].onThree();
    dir.position[5].dirty = true;
    cout << "\t==== Directory contents ====\n\n" << dir << "\n";

}
