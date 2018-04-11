// rpcutils.h
//
// Declares utility functions for both rpc proxies and stubs
//
// by: Justin Jo and Charles Wan

#ifndef _RPCUTILS_H_
#define _RPCUTILS_H_

#include "c150streamsocket.h"
#include "funcheader.h"

using namespace std;
using namespace C150NETWORK;


// to be returned ahead of function return values to indicate succ or fail
enum ResultCode {
    success = 0,
    general_failure = -1,
    nonexistent_function = -2
};


void initDebugLog(const char *logname, const char *progname, uint32_t classes);

int readInt(C150StreamSocket *sock);
void writeInt(C150StreamSocket *sock, int n);
float readFloat(C150StreamSocket *sock);
void writeFloat(C150StreamSocket *sock, float n);
string readString(C150StreamSocket *sock, size_t strlen);
string readString(C150StreamSocket *sock);
void writeString(C150StreamSocket *sock, const string &s);
void readFuncHeader(C150StreamSocket *sock, FuncHeader &hdr);
void writeFuncHeader(C150StreamSocket *sock, const FuncHeader &hdr);
ResultCode readResultCode(C150StreamSocket *sock);
void writeResultCode(C150StreamSocket *sock, ResultCode code);


#endif
