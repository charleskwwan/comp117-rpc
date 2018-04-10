// funcheader.h
//
// Defines the FuncHeader class, which holds information about functions
// being called
//
// by: Justin Jo and Charles Wan

#ifndef _FUNCHEADER_H_
#define _FUNCHEADER_H_

#include <vector>

using namespace std;


struct FuncHeader {
    string funcname;
    vector<unsigned> argsSizes;
};

#endif
