/*******************************************************
                          main.cc
                  Ahmad Samih & Yan Solihin
                           2009
                {aasamih,solihin}@ece.ncsu.edu
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

#include "cache.h"
int NODES = 4;
int DIRECTORY_SIZE = 0;

int main(int argc, char *argv[]) {

    ifstream fin;
    FILE * pFile;

    if (argv[1] == NULL) {
        printf("input format: ");
        printf("./smp_cache <cache_size> <assoc> <block_size> <num_processors> <protocol> <trace_file> \n");
        exit(0);
    }

    /*
        In order to run your simulator, you need to execute the following command: 
        ./smp_cache <cache_size> <block_size> <num_processors> <protocol> <trace_file> 
     
        Where: 
     
        smp_cache: Executable of the SMP simulator generated after making 
        cache_size: Size of each cache in the system (all caches are of the same size) 
        assoc: Associativity of each cache (all caches are of the same associativity) 
        block_size: Block size of each cache line (all caches are of the same block size) 
        num_processors: Number of processors in the system (represents how many caches should be instantiated) 
        protocol: Coherence protocol to be used (0: Firefly, 1:Dragon) 
        trace_file: The input file that has the multi threaded workload trace. 
    
    */ 
     
    /*****uncomment the next five lines*****/
    int cache_size = atoi(argv[1]);
    int cache_assoc= atoi(argv[2]);
    int blk_size   = atoi(argv[3]);
    //int num_processors = atoi(argv[4]);/*1, 2, 4, 8*/
    //int protocol   = atoi(argv[5]);	 /*0: Firefly, 1:Dragon*/
    char *fname = (char *) malloc(20);
    fname = argv[4];
    DIRECTORY_SIZE = cache_size * NODES / blk_size;

    //****************************************************//
    //**printf("===== Simulator configuration =====\n");**//
    //*******print out simulator configuration here*******//
    //****************************************************//

    printf("===== 506 SMP Simulator Configuration =====\n");
    printf("L1_SIZE:               %i\n", cache_size);
    printf("L1_ASSOC:              %i\n", cache_assoc);
    printf("L1_BLOCKSIZE:          %i\n", blk_size);
    printf("NUMBER OF PROCESSORS:  %i\n", NODES);
    
/*    string protocol_name;
    
    if (protocol == 0) {
        protocol_name = "Firefly";
    } else {
        protocol_name = "Dragon";
    }
    
    printf("COHERENCE PROTOCOL:    %s\n", protocol_name.c_str());  
*/
    printf("TRACE FILE:            %s\n", fname);
    
    //*********************************************//
    //*****create an array of caches here**********//
    //*********************************************//	
    vector<FBV> directory;
    vector<Cache*> cachesArray;
    
    for (int i = 0; i < NODES; i++) {
        cachesArray.push_back(new Cache(cache_size, cache_assoc, blk_size));
    }
    
    for (int i = 0; i < directory_size; i++)  {
      directory.push_back(new directory);
      directory[i].flags = UNOWNED;
      directory[i].state = 0;
      directory[i].dirty = false;
    }
    
    
    pFile = fopen(fname, "r");
    if (pFile == 0) {
        printf("Trace file problem\n");
        exit(0);
    }
    
    ///******************************************************************//
    //**read trace file,line by line,each(processor#,operation,address)**//
    //*****propagate each request down through memory hierarchy**********//
    //*****by calling cachesArray[processor#]->Access(...)***************//
    ///******************************************************************//
    
    int processor_number;
    uchar op;
    ulong address;
    
    while (fscanf(pFile, "%i %c %lx", &processor_number, &op, &address) == 3) {
        cachesArray[processor_number]->Access(address, op, cachesArray, directory, processor_number);
    }
    
    
    fclose(pFile);

    //********************************//
    //print out all caches' statistics //
    //********************************//

    for (int i = 0; i < NODES; i++) {
        printf("===== Simulation results (Cache_%i)      =====\n", i);
        cachesArray[i]->printStats();
    }
}
