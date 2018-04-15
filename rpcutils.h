// rpcutils.h
//
// Declares utility functions for both rpc proxies and stubs
//
// by: Justin Jo and Charles Wan

#ifndef _RPCUTILS_H_
#define _RPCUTILS_H_

#include <sstream>
#include "c150streamsocket.h"
#include "c150exceptions.h"

using namespace std;
using namespace C150NETWORK;


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


enum StatusCode {
    // 000 range - general
    success = 0,
    incomplete_bytes = 1, // unexpected number of bytes received

    // 100 range - function names
    existing_func = 100,
    nonexistent_func = 101,

    // 200 range - arguments
    good_args = 200,
    too_few_args = 201,
    too_many_args = 202,
    scrambled_args = 203 // correct number of bytes, badly organized

    // 300 range - return values
};


void initDebugLog(const char *logname, const char *progname, uint32_t classes);

// void writeSock(C150StreamSocket *sock, char *)
// ssize_t readSock(C150StreamSocket *sock, char *buf, ssize_t lenToRead);
StatusCode readAndCheck(C150StreamSocket *sock, char *buf, ssize_t lenToRead);
void readAndThrow(C150StreamSocket *sock, char *buf, ssize_t lenToRead);
StatusCode checkArgs(stringstream &ss);
string extractString(stringstream &ss);


void print_bytes(const void *object, size_t size);


#endif
