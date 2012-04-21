#ifndef ENTRY_H
#define ENTRY_H

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

extern int DEBUG;
extern int NODES;

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned int uint;

class entry {

    enum {
        EXCLUSIVE_MODIFIED = 0,
        SHARED,
        UNOWNED
    };

public:

    entry() {
    } // Default constructor

    ~entry() {
    } // Default destructor

    friend ostream &operator<<(ostream &stream, entry obj);

    // initialize directory line to empty state

    void clearAll() {
        tag = 0;
        state = UNOWNED;
        dirty = false;
        for (int i = 0; i < NODES; i++) processor[i] = 0;
        if (DEBUG) {
            cout << "In the definition of entry, where we just created:\n";
            cout << "\t\t" << tag << ", " << state << ", " << dirty << " , ";
            for (int i = 0; i < NODES; i++) cout << processor[i] << ", ";
            cout << "\n";
        }
    }

    // Self-explanatory getters and setters

    void setStateEM() {
        state = EXCLUSIVE_MODIFIED;
    }

    void setStateS() {
        state = SHARED;
    }

    bool isEM() {
        return state == EXCLUSIVE_MODIFIED;
    }

    bool isS() {
        return state == SHARED;
    }

    bool isU() {
        return state == UNOWNED;
    }

    bool isInProcCache(int pn) {
        return processor[pn] == 1;
    }

    void processorOn(int p) {
        processor[p] = 1;
    }

    void processorOff(int p) {
        processor[p] = 0;
        if (tagNoLongerCached()) clearAll();
    }

    void setTag(ulong addr) {
        tag = addr;
    }

    ulong getTag() {
        return tag;
    }

    void setState(int pstate) {
        state = pstate;
    }

    int getState() {
        return state;
    }

    bool isDirty() {
        return dirty;
    }

    void setDirty() {
        dirty = true;
    }

    void setClean() {
        dirty = false;
    }

    bool tagNoLongerCached() {
        for (int i = 0; i < NODES; ++i) {
            if (processor[i] == 1) return false;
        }
        if (DEBUG) cout << "Directory entry transitioning to Unowned\n";
        return true;
    }

    ulong tag;
    int processor[4];
    int state;
    bool dirty;
};

#endif