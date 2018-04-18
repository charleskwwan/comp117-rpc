// rpcutils.h
//
// Declares utility functions for both rpc proxies and stubs
//
// by: Justin Jo and Charles Wan

#ifndef _RPCUTILS_H_
#define _RPCUTILS_H_

#include <sstream>
#include <inttypes.h>
#include "c150streamsocket.h"
#include "c150exceptions.h"

using namespace std;
using namespace C150NETWORK;


// N
//  - union to help handle endianness problem by allowing type punning

union N {
    uint32_t u; // for inet ops
    int i;
    float f;
    char c[4]; // for read/write over network
};


// RPCStubException
//  - Error during rpc proxy/stub functions
//  - Can be caught and handled by proxy/stub if want to avoid crashing client/
//    server

class RPCException : public C150Exception {
private:
protected:
    // constructor to pass name and explanation
    RPCException(string exceptionName, string explain) :
        C150Exception(exceptionName, explain) 
    {};

public:
    // constructor: takes just explain
    RPCException(string explain) :
        C150Exception("RPCException", explain)
    {};

    // lazy constructor with no explain
    RPCException() :
        C150Exception(
            "RPCException",
            "RPCException thrown without explanation"
        )
    {};

    // virtual destructor for classes with virtual methods
    virtual ~RPCException() {};
};


// StatusCode
//  - used by stub to communicate to proxy whether or not values sent were
//    accepted or not

enum StatusCode {
    // 000 range - general
    success = 0,
    incomplete_bytes = 1, // unexpected number of bytes received
    no_null_term_found = 2,
    timed_out = 3,

    // 100 range - function names
    existing_func = 100,
    nonexistent_func = 101,

    // 200 range - arguments/results
    good_bytes = 200,
    too_many_bytes = 201,
    too_few_bytes = 202,
    scrambled_bytes = 203 // correct number of bytes, badly organized
};


// constants
const uint32_t VARDEBUG = 0x00000001; // debug flag for variables read/written


// function declarations
void initDebugLog(const char *logname, const char *progname, uint32_t classes);
void logDebug(stringstream &debugStream, uint32_t debugClasses, bool grade);
void logThrow(stringstream &debugStream, uint32_t debugClasses, bool grade);
void printBytes(const unsigned char *buf, size_t buflen);

StatusCode readAndCheck(C150StreamSocket *sock, char *buf, ssize_t lenToRead);
void readAndThrow(C150StreamSocket *sock, char *buf, ssize_t lenToRead);
void writeAndCheck(C150StreamSocket *sock, const char *buf, ssize_t lenToWrite);
StatusCode checkBytes(stringstream &ss);
string debugStatusCode(StatusCode code);

int extractInt(stringstream &ss);
float extractFloat(stringstream &ss);
string extractString(stringstream &ss);
int readInt(C150StreamSocket *sock);
float readFloat(C150StreamSocket *sock);
void writeInt(C150StreamSocket *sock, int i);
void writeFloat(C150StreamSocket *sock, float f);
void writeString(C150StreamSocket *sock, const string &s);

#endif
