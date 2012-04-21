/*******************************************************
                          cache.cc
                  Ahmad Samih & Yan Solihin
                           2009
                {aasamih,solihin}@ece.ncsu.edu
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "cache.h"
using namespace std;

Cache::Cache(int s, int a, int b) {
    ulong i, j;

    reads = readMisses = writes = invalidations = 0;
    writeMisses = writeBacks = currentCycle = 0;
    cacheToCacheTransfers = missingLines = 0;

    size = (ulong) (s);
    assoc = (ulong) (a);
    lineSize = (ulong) (b);
    sets = (ulong) ((s / b) / a);
    numLines = (ulong) (s / b);
    log2Sets = (ulong) (log2(sets));
    log2Blk = (ulong) (log2(b));

    //*******************//
    //initialize your counters here//
    //*******************//

    tagMask = 0;
    for (i = 0; i < log2Sets; i++) {
        tagMask <<= 1;
        tagMask |= 1;
    }

    /**create a two dimentional cache, sized as cache[sets][assoc]**/
    cache = new cacheLine*[sets];
    for (i = 0; i < sets; i++) {
        cache[i] = new cacheLine[assoc];
        for (j = 0; j < assoc; j++) {
            cache[i][j].invalidate();
        }
    }

}

/**you might add other parameters to Access()
since this function is an entry point
to the memory hierarchy (i.e. caches)**/
void Cache::Access(ulong addr, uchar op, vector<Cache*> &cachesArray, directory &dir, int proc_num) {
    ulong tag;

    currentCycle++; /*per cache global counter to maintain LRU order
                     among cache ways, updated on every cache access*/

    if (op == 'w') {
        writes++;
    }
    else {
        reads++;
    }

    cacheLine *line = findLine(addr);
    tag = calcTag(addr);

    if (line == NULL || line->getFlags() == INVALID) { // --=== THESE ARE CACHE MISSES ===--
        int index = dir.findTagPos(tag); // search directory for index of tag (-1 denotes not found)
        if (op == 'r') { // READ request
            readMisses++; // STATS:  record read miss
            if (index < 0) { // Case:  cache miss, directory miss (read)
                cacheLine *newline = fillLine(addr, dir, proc_num); // put line in cache            
                int insert_pos = dir.findUnownedPos(); // find first open directory position
                dir.position[insert_pos].setTag(tag); // set directory tag
                dir.position[insert_pos].processorOn(proc_num); // turn this processor bit on
                dir.position[insert_pos].setStateEM(); // set directory to EXCLUSIVE_MODIFIED state
                newline->setFlags(EXCLUSIVE); // set cache state to EXCLUSIVE
            } else { // Case:  cache miss, directory hit (read)
                cacheLine *newline = fillLine(addr, dir, proc_num); // put line in cache
                bool wasCacheToCacheTranfer = false;
                for (int i = 0; i < NODES; i++) { // loop through processors in directory
                    if (proc_num != i && dir.position[index].isInProcCache(i)) { // change all in directory to SHARED state 
                        cacheLine *shared_line = cachesArray[i]->findLine(addr);
                        if (shared_line != NULL) { // NULL pointer protection
                            if (shared_line->getFlags() == EXCLUSIVE) {
                                wasCacheToCacheTranfer = true;
                            }
                            shared_line->setFlags(SHARED);
                        } else {
							missingLines++;
						}
                    }
                }
                if (wasCacheToCacheTranfer) {
                    cacheToCacheTransfers++; // STATS:  record transfer from cache to cache
                }
				dir.position[index].setTag(tag); // set directory tag
				dir.position[index].processorOn(proc_num); // turn on this processor in directory
                dir.position[index].setStateS(); // set directory to SHARED state
                newline->setFlags(SHARED); // set cache state to SHARED
            }
        } else { // WRITE request
            writeMisses++; // STATS:  record write miss
            if (index < 0) { // Case:  cache miss, directory miss (write)
                cacheLine *newline = fillLine(addr, dir, proc_num); // put line in cache
                int insert_pos = dir.findUnownedPos(); // find first open directory position
                dir.position[insert_pos].setTag(tag); // set directory tag
                dir.position[insert_pos].processorOn(proc_num); // turn on this processor bit
                dir.position[insert_pos].setStateEM(); // set directory state to EXCLUSIVE_MODIFIED
                newline->setFlags(MODIFIED); // set cache state to modified
            } else { // Case:  cache miss, directory hit (write)
                cacheLine *newline = fillLine(addr, dir, proc_num); // fill cache line
                for (int i = 0; i < NODES; i++) { // INVALIDate all other processor cache entries
                    if (proc_num != i && dir.position[index].isInProcCache(i)) {
                        cacheLine *invalid_line = cachesArray[i]->findLine(addr);
                        if (invalid_line != NULL) {
                            invalid_line->invalidate();
                            cachesArray[i]->recordInvalidation(); // record invalidation in proper cache
                        } else {
							missingLines++;
						}
                        dir.position[index].processorOff(i); // turn off invalid pocessor bits
                    }
                }
				cacheToCacheTransfers++; // STATS:  record inter-cache transfers
				dir.position[index].setTag(tag); // set directory tag
				dir.position[index].processorOn(proc_num); // turn on this processor bit
                dir.position[index].setStateEM(); // set directory state to EXCLUSIVE_MODIFIED
                newline->setFlags(MODIFIED); // set cache flag
            }
        }
    } else { // --=== THESE ARE CACHE HITS ===--
        int index = dir.findTagPos(tag);
        if (op == 'r') { // READ request
            // Reading from your own cache on a hit won't do anything (line already tested as valid)
            // Case:  cache hit (will never check directory)
        } else { // WRITE request
            if (index < 0) { // Case:  cache hit, directory miss (write)
                //cout << "THIS SHOULD PROBABLY NEVER HAPPEN\n\n";
                //dir.position[index].setTag(tag);               // set directory tag
                //dir.position[index].processorOn(proc_num);     // turn on this processor bit
                //dir.position[index].setStateEM();              // set directory state to EXCLUSIVE_MODIFIED
                // if (DEBUG == 2) cout << "Current addr:  " << addr << endl;
                // inCacheNotDirectory++;
                // cacheLine *newline = findLine(addr);
                // newline->setFlags(MODIFIED);
            } else { // Case:  cache hit, directory hit (write)
                if (line->getFlags() == EXCLUSIVE || line->getFlags() == MODIFIED) {
                    line->setFlags(MODIFIED); // silent E -> M transition
                } else { // Shared cache line
                    for (int i = 0; i < NODES; i++) { // set lines in other processors to INVALID
                        if (proc_num != i && dir.position[index].isInProcCache(i)) {
                            cacheLine *line_invalid = cachesArray[i]->findLine(addr);
                            if (line_invalid != NULL) {
                                line_invalid->invalidate();
                                cachesArray[i]->recordInvalidation(); // record invalidation in proper cache
                            }
                            dir.position[index].processorOff(i); // set processor bits to zero
                        }
                    }
                    dir.position[index].setStateEM(); // set directory to EXCLUSIVE_MODIFIED
                    line->setFlags(MODIFIED); // set cache to MODIFIED state
                }
            }
        }
    }
}

/*look up line*/
cacheLine *Cache::findLine(ulong addr) {
    ulong i, j, tag, pos;

    pos = assoc;
    tag = calcTag(addr);
    i = calcIndex(addr);

    for (j = 0; j < assoc; j++) {
        if (cache[i][j].isValid()) {
            if (cache[i][j].getTag() == tag) {
                pos = j;
                break;
            }
        }
    }

    if (pos == assoc) {
        return NULL;
    } else {
        return &(cache[i][pos]);
    }
}

/*upgrade LRU line to be MRU line*/
void Cache::updateLRU(cacheLine *line) {
    line->setSeq(currentCycle);
}

/*return an invalid line as LRU, if any, otherwise return LRU line*/
cacheLine * Cache::getLRU(ulong addr) {
    ulong i, j, victim, min;

    victim = assoc;
    min = currentCycle;
    i = calcIndex(addr);

    for (j = 0; j < assoc; j++) {
        if (cache[i][j].isValid() == 0) return &(cache[i][j]);
    }

    for (j = 0; j < assoc; j++) {
        if (cache[i][j].getSeq() <= min) {
            victim = j;
            min = cache[i][j].getSeq();
        }
    }
    assert(victim != assoc);
    return &(cache[i][victim]);
}

/*find a victim, move it to MRU position*/
cacheLine *Cache::findLineToReplace(ulong addr) {
    cacheLine * victim = getLRU(addr);
    updateLRU(victim);

    return (victim);
}

/*allocate a new line*/
cacheLine *Cache::fillLine(ulong addr, directory &dir, int pnum) {
    ulong tag = calcTag(addr);

    cacheLine *victim = findLineToReplace(addr);
    if (victim->getFlags() == INVALID) { // on cache eviction
        int index = dir.findTagPos(tag); // find position in directory
        if (index >= 0) { // if it is in directory
            dir.position[index].processorOff(pnum); // turn off this processor bit
        }
    }
    assert(victim != 0);
    if (victim->getFlags() == MODIFIED) {
        // STATS:  writebacks handled inline in header
        writeBack(addr);
    }

    victim->setTag(tag);
    victim->setFlags(INVALID);
    /**note that this cache line has been already
     upgraded to MRU in the previous function (findLineToReplace)**/

    return victim;
}

void Cache::printStats() {

    //printf("===== Simulation results      =====\n");
    /****print out the rest of statistics here.****/
    /****follow the ouput file format**************/

    printf("01. number of reads:                              %li\n", reads);
    printf("02. number of read misses:                        %li\n", readMisses);
    printf("03. number of writes:                             %li\n", writes);
    printf("04. number of write misses:                       %li\n", writeMisses);
    printf("05. total miss rate:                              %f\n", (writeMisses + readMisses + 0.0) / (reads + writes));
    printf("06. number of writebacks:                         %li\n", writeBacks);
    printf("07. number of invalidations:                      %li\n", invalidations);
    printf("08. number of cache to cache transfers:           %li\n", cacheToCacheTransfers);
    printf("09. missing lines:              				  %li\n", missingLines);
}
