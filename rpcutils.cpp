// rpcutils.cpp
//
// Defines utility functions for both rpc proxies and stubs
//
// by: Justin Jo and Charles Wan


#include "c150streamsocket.h"
#include "funcheader.h"
#include "streambuffer.h"
#include "rpcutils.h"

using namespace std;
using namespace C150NETWORK;


// initDebugLog
//      - enables logging to either console or file
//
// args:
//      - logname: name of log file; if NULL, defaults to console
//      - progname: name of program
//      - classes: for which to log
//
// returns: n/a

void initDebugLog(const char *logname, const char *progname, uint32_t classes) {
    if (logname != NULL) { // pipe logging to file
        ofstream *outstreamp = new ofstream(logname);
        DebugStream *filestreamp = new DebugStream(outstreamp);
        DebugStream::setDefaultLogger(filestreamp);
    }

    c150debug->setPrefix(progname);
    c150debug->enableTimestamp();

    c150debug->enableLogging(classes);
}


// readInt
//  - uses a StreamBuffer to read an int from a stream socket

int readInt(C150StreamSocket *sock) {
    StreamBuffer buf;
    buf.read(sock, 4);
    int n = *(int *)buf.c_str();

    c150debug->printf(
        C150APPLICATION,
        "rpcutils.readInt: read int %d", n
    );

    return n;
}


// writeInt
//  - writes an int to a stream socket

void writeInt(C150StreamSocket *sock, int n) {
    StreamBuffer buf((const char *)&n, 4);
    c150debug->printf(
        C150APPLICATION,
        "rpcutils.writeInt: writing integer %d", n
    );
    buf.write(sock);
}


// readFloat
//  - uses a StreamBuffer to read an float from a stream socket

float readFloat(C150StreamSocket *sock) {
    StreamBuffer buf;
    buf.read(sock, 4);
    int n = *(float *)buf.c_str();

    c150debug->printf(
        C150APPLICATION,
        "rpcutils.readFloat: read float %f", n
    );

    return n;
}


// writeFloat
//  - writes a float to a stream socket

void writeFloat(C150StreamSocket *sock, float n) {
    StreamBuffer buf((const char *)&n, 4);
    c150debug->printf(
        C150APPLICATION,
        "rpcutils.writeFloat: writing float %f", n
    );
    buf.write(sock);
}


// readString
//  - uses a StreamBuffer to read a string of a given length from a stream
//    socket
//  - strlen should account for the null terminator

string readString(C150StreamSocket *sock, size_t strlen) {
    StreamBuffer buf;
    buf.read(sock, strlen);

    c150debug->printf(
        C150APPLICATION,
        "rpcutils.readString: read string '%s'", buf.c_str()
    );

    return string(buf.c_str());
}

// readString (v2), reads until null terminator found
string readString(C150StreamSocket *sock) {
    return readString(sock, 0);
}


// writeString
//  - writes a string to a stream socket, including the null terminator

void writeString(C150StreamSocket *sock, const string &s) {
    StreamBuffer buf(s.c_str(), s.length() + 1);
    c150debug->printf(
        C150APPLICATION,
        "rpcutils.writeString: writing string '%s'", s.c_str()
    );
    buf.write(sock);
}


// readFuncHeader
//  - reads a function header from a stream socket
//  - reads in the following order:
//      - function name
//      - number of arguments
//      - sizes of each argument

void readFuncHeader(C150StreamSocket *sock, FuncHeader &hdr) {
    hdr.funcname = readString(sock); // read until null
    int nargs = readInt(sock);

    hdr.argsSizes.clear(); // must empty

    for (int i = 0; i < nargs; i++) {
        hdr.argsSizes.push_back(readInt(sock));
    }
}


// writeFuncHeader
//  - writes a function header to a stream socket
//  - reads in the following order:
//      - function name
//      - number of arguments
//      - sizes of each argument

void writeFuncHeader(C150StreamSocket *sock, const FuncHeader &hdr) {
    writeString(sock, hdr.funcname);
    writeInt(sock, hdr.argsSizes.size()); // number of arguments

    for (size_t i = 0; i < hdr.argsSizes.size(); i++) {
        writeInt(sock, hdr.argsSizes[i]);
    }
}


// readResultCode
//  - reads a status code from a stream socket
//  - a debug message is printed to match the success/failure nature of the code

ResultCode readResultCode(C150StreamSocket *sock) {
    ResultCode code = (ResultCode)readInt(sock);
    c150debug->printf(
        C150RPCDEBUG,
        "rpcutils: status code %d received, indicating %s",
        code, code < 0 ? "failure" : "success"
    );
    return code;
}


// writeResultCode
//  - writes a status code to a stream socket
//  - a debug message is printed to match the success/failure nature of the code

void writeResultCode(C150StreamSocket *sock, ResultCode code) {
    c150debug->printf(
        C150RPCDEBUG,
        "rpcutils: status code %d sent, indicating %s",
        code, code < 0 ? "failure" : "success"
    );
    writeInt(sock, code);
}
