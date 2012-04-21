/*******************************************************
                          cache.h
                  Ahmad Samih & Yan Solihin
                           2009
                {aasamih,solihin}@ece.ncsu.edu
 ********************************************************/

#ifndef CACHE_H
#define CACHE_H

#include <cmath>
#include <iostream>
#include <vector>
#include "directory.h"

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned int uint;
extern int NODES; // global from main
//extern directory dir;               // put directory in global scope of cache, hackish but avoids moderate refactor

/****add new states, based on the protocol****/

enum {
    EXCLUSIVE = 0,
    MODIFIED,
    SHARED,
    INVALID
};

class cacheLine {
protected:
    ulong tag;
    ulong Flags; // 0:exclusive, 1:modified, 2:shared, 3:invalid 
    ulong seq;

public:

    cacheLine() {
        tag = 0;
        Flags = INVALID;
    }

    ulong getTag() {
        return tag;
    }

    ulong getFlags() {
        return Flags;
    }

    ulong getSeq() {
        return seq;
    }

    void setSeq(ulong Seq) {
        seq = Seq;
    }

    void setFlags(ulong flags) {
        Flags = flags;
    }

    void setTag(ulong a) {
        tag = a;
    }

    void invalidate() {
        tag = 0;
        Flags = INVALID;
    } //useful function

    bool isValid() {
        return ((Flags) != INVALID);
    }
};

class Cache {
protected:
    ulong size, lineSize, assoc, sets, log2Sets, log2Blk, tagMask, numLines;
    ulong reads, readMisses, writes, writeMisses, writeBacks, invalidations;
    ulong cacheToCacheTransfers, missingLines;
    ulong inCacheNotDirectory;
    ulong protocol;

    //******///
    //add coherence counters here///
    //******///

    cacheLine **cache;

    ulong calcTag(ulong addr) {
        return (addr >> (log2Blk));
    }

    ulong calcIndex(ulong addr) {
        return ((addr >> log2Blk) & tagMask);
    }

    ulong calcAddr4Tag(ulong tag) {
        return (tag << (log2Blk));
    }

public:
    ulong currentCycle;

    Cache(int, int, int);

    ~Cache() {
        delete cache;
    }

    cacheLine *findLineToReplace(ulong addr);
    cacheLine *fillLine(ulong addr, directory &dir, int proc_num);
    cacheLine *findLine(ulong addr);
    cacheLine *getLRU(ulong);

    void recordInvalidation() {
        ++invalidations;
    }

    ulong getRM() {
        return readMisses;
    }

    ulong getWM() {
        return writeMisses;
    }

    ulong getReads() {
        return reads;
    }

    ulong getWrites() {
        return writes;
    }

    ulong getWB() {
        return writeBacks;
    }

    void writeBack(ulong) {
        writeBacks++;
    }

    void Access(ulong, uchar, std::vector<Cache*> &, directory &, int);
    void printStats();
    void updateLRU(cacheLine *);

    //******///
    //add other functions to handle bus transactions///
    //******///

    //bool busUpd(ulong, std::vector<Cache*> &);
    //bool busRd(ulong, std::vector<Cache*> &);
    //bool busUpd(ulong);
    //bool busRd(ulong);

};

#endif
