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

    reads = readMisses = writes = 0;
    writeMisses = writeBacks = currentCycle = 0;
    memoryTransactions = cacheToCacheTransfers = 0;

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
   currentCycle++; /*per cache global counter to maintain LRU order
                        among cache ways, updated on every cache access*/

   if (op == 'w') { writes++; } 
   else { reads++; }

   cacheLine *line = findLine(addr);
    
   if (line == NULL || line->getFlags() == INVALID)  {     // --=== THESE ARE CACHE MISSES ===--
      int dir_pos = dir.findTagPos(addr);
      if (op == 'r')  {                // READ request
         if (dir_pos < 0)  {              // Case:  cache miss, directory miss (read)
            int insert_pos = dir.findUnownedPos();
            dir[insert_pos].setTag(addr);
            dir[insert_pos].processorOn(proc_num);
            dir[insert_pos].setState(EXCLUSIVE_MODIFIED);
            cacheLine *newline = fillLine(addr);
            newline->setFlags(EXCLUSIVE);
            memoryTransactions++;
         }
         else  {                          // Case:  cache miss, directory hit (read)
            dir[dir_pos].processorOn(proc_num);    // turn on this processor in directory
            dir[dir_pos].setState(SHARED);         // set directory to SHARED state
            cacheLine *newline = fillLine(addr);   // put memory in cache
            newline->setFlags(SHARED);             // set cache flag to SHARED
            for (int i = 0; i < NODES; ++i)  {         // loop through processors in FBV
               if (dir.isInProcCache(i))  {               // change all to SHARED state except this one
                  cacheLine *shared_line = cachesArray[i].findLine(addr);
                  if (shared_line != NULL)  {
                     shared_line->setState(SHARED);
                  }
               }
            }
         }
      }
      else  {                 // WRITE request
         if (dir_pos < 0)  {     // Case:  cache miss, directory miss (write)
            int insert_pos = dir.findUnownedPos();
            dir[insert_pos].setTag(addr);
            dir[insert_pos].processorOn(proc_num);
            dir[insert_pos].setState(EXCLUSIVE_MODIFIED);
            dir[insert_pos].setDirty();
            cacheLine *newline = fillLine(addr);
            newline->setFlags(MODIFIED);
            ++memoryTransactions;
         }
         else  {                 // Case:  cache miss, directory hit (write)
            for (int i = 0, i < NODES; ++i)  {        // set lines in other processors to INVALID
               if (dir[insert_pos].isInProcCache(i))  {
                  cacheLine *invalid_line = cachesArray[i].findLIne(addr);
                  if (invalid_line != NULL)  {
                     invalid_line->setState(INVALID);
                  }
                  dir[insert_pos].processorOff(i);          // set processor bit to zero
               }
            }
            dir[insert_pos].setTag(addr);                   // set DIRECTORY tag
            dir[insert_pos].processorOn(proc_num);          // set DIRECTORY processor bit
            dir[insert_pos].setState(EXCLUSIVE_MODIFIED);   // set DIRECTORY state
            dir[insert_pos].setDirty();                     // set DIRECTORY entry as dirty
            cacheLine *newline = fillLine(addr);            // fill cache line
            newline->setFlags(MODIFIED);                    // set cache flag
            ++memoryTransactions;                           // count memory transaction
         }
      }
   }
   else  {                 // --=== THESE ARE CACHE HITS ===--
      int dir_pos = dir.findTagPos(addr);
      if (op == 'r')  {                // READ request
         // Reading from your own cache on a hit won't do anything (line already tested as valid)
         // Case:  cache hit (will never check directory)
      }
      else  {                          // WRITE request
         if (dir_pos < 0)  {              // Case:  cache hit, directory miss (write)
            cout << "THIS SHOULD PROBABLY NEVER HAPPEN\n\n";
         }
         else  {                          // Case:  cache hit, directory hit (write)
            if (line->getFlags() == EXCLUSIVE || line->getFlags() == MODIFIED)  {
               line->setFlags(MODIFIED);           // silent E -> M transition
            }
            else  {                          // Shared cache line
               for (int i = 0, i < NODES; ++i)  {        // set lines in other processors to INVALID
                  if (num_proc != i && dir[insert_pos].isInProcCache(i))  {
                     cacheLine *line_invalid = cachesArray[i].findLIne(addr);
                     if (line_invalid != NULL)  {
                        line_invalid->setState(INVALID);
                     }
                     dir[insert_pos].processorOff(i);          // set processor bit to zero
                  }
               }
               dir.[insert_pos].setState(EXCLUSIVE_MODIFIED);
               dir[insert_pos].setDirty();
               line->setFlags(MODIFIED);
            }
         }
      }
   }
}
    
/*
    bool snoop = false;
    if (line == NULL || line->getFlags() == INVALID) { //miss
        cacheLine *newline = fillLine(addr);

        if (op == 'w') {
            snoop = busRd(addr, cachesArray);
            if (snoop) {
                busUpd(addr, cachesArray);
                if (protocol == 0) {
                    newline->setFlags(SHARED_CLEAN);
                    memoryTransactions++;
                } else {
                    newline->setFlags(SHARED_MODIFIED);
                }
                cacheToCacheTransfers++;
            } else {
                newline->setFlags(MODIFIED);
                memoryTransactions++;
            }
            writeMisses++;
        } else {
            snoop = busRd(addr, cachesArray);
            if (snoop) {
                newline->setFlags(SHARED_CLEAN);
                cacheToCacheTransfers++;
            } else {
                newline->setFlags(EXCLUSIVE);
                memoryTransactions++;
            }
            readMisses++;
        }
    } else {
        //since it's a hit, update LRU and update dirty flag
        updateLRU(line);

        if (op == 'w') {
            if (line->getFlags() == EXCLUSIVE) {
                line->setFlags(MODIFIED);
            } else if (line->getFlags() != MODIFIED) {
                if (protocol == 0) {
                    line->setFlags(INVALID);
                    snoop = busUpd(addr, cachesArray);
                    if (snoop) {
                        line->setFlags(SHARED_CLEAN);
                    } else {
                        line->setFlags(EXCLUSIVE);
                    }
                    memoryTransactions++;
                } else {
                    line->setFlags(INVALID);
                    snoop = busUpd(addr, cachesArray);
                    if (snoop) {
                        line->setFlags(SHARED_MODIFIED);
                    } else {
                        line->setFlags(MODIFIED);
                    }
                }
            }
        }
    }
*/
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
    
    //============================
    // DO DIRECTORY EVICTION HERE|
    //============================
    
    
    return &(cache[i][victim]);
}

/*find a victim, move it to MRU position*/
cacheLine *Cache::findLineToReplace(ulong addr) {
    cacheLine * victim = getLRU(addr);
    updateLRU(victim);

    return (victim);
}

/*allocate a new line*/
cacheLine *Cache::fillLine(ulong addr) {
    ulong tag;

    cacheLine *victim = findLineToReplace(addr);
    assert(victim != 0);
    if (victim->getFlags() == MODIFIED || victim->getFlags() == SHARED_MODIFIED) {
        writeBack(addr);
    }

    tag = calcTag(addr);
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
    printf("07. number of memory transactions:                %li\n", memoryTransactions);
    printf("08. number of cache to cache transfers:           %li\n", cacheToCacheTransfers);
}


/*
bool Cache::busUpd(ulong addr, std::vector<Cache*> &cachesArray) {
    bool snoop = false;

    for (unsigned int i = 0; i < cachesArray.size(); i++) {
        snoop = cachesArray[i]->busUpd(addr) || snoop;
    }

    return snoop;
}

bool Cache::busRd(ulong addr, std::vector<Cache*> &cachesArray) {
    bool snoop = false;

    for (unsigned int i = 0; i < cachesArray.size(); i++) {
        snoop = cachesArray[i]->busRd(addr) || snoop;
    }

    return snoop;
}

bool Cache::busUpd(ulong addr) {
    cacheLine *line = findLine(addr);
    if (line == NULL || line->getFlags() == INVALID) {
        return false;
    } else {
        if (protocol == 0) {
            if (line->getFlags() == MODIFIED) {
                memoryTransactions++;
            }
            line->setFlags(SHARED_CLEAN);
            return true;
        } else {
            if (line->getFlags() == SHARED_CLEAN || line->getFlags() == SHARED_MODIFIED) {
                line->setFlags(SHARED_CLEAN);
                return true;
            } else { // EXLUSIVE and MODIFIED don't care.
                return false;
            }
        }
    }
}

bool Cache::busRd(ulong addr) {
    cacheLine *line = findLine(addr);

    if (line == NULL || line->getFlags() == INVALID) {
        return false;
    } else {
        if (protocol == 0) {
            if (line->getFlags() == MODIFIED) {
                memoryTransactions++;
            }
            line->setFlags(SHARED_CLEAN);
            return true;
        } else {
            if (line->getFlags() == EXCLUSIVE || line->getFlags() == SHARED_CLEAN) {
                line->setFlags(SHARED_CLEAN);
                return true;
            } else {
                line->setFlags(SHARED_MODIFIED);
                return true;
            }
        }
    }
}
*/