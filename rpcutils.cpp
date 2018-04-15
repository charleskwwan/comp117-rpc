// rpcutils.cpp
//
// Defines utility functions for both rpc proxies and stubs
//
// by: Justin Jo and Charles Wan


#include <string>
#include <sstream>
#include "c150streamsocket.h"
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


// readAndCheck
//  - reads lenToRead number of bytes from sock, and returns a status code based
//    on whether or not the bytes were successfully read
//
//  returns:
//      - success, exact bytes receive
//      - incomplete_bytes, wrong number of bytes read

StatusCode readAndCheck(C150StreamSocket *sock, char *buf, ssize_t lenToRead) {
    ssize_t readlen = sock->read(buf, lenToRead);

    if (readlen == lenToRead) {
        c150debug->printf(
            C150APPLICATION,
            "rpcutils.readAndCheck: read exactly %d bytes, as expected",
            readlen
        );
        return success;

    } else {
        c150debug->printf(
            C150APPLICATION,
            "rpcutils.readAndCheck: %d bytes could not be read, got %d "
            "instead", lenToRead, readlen
        );
        return incomplete_bytes;
    }
}


// readAndThrow
//  - reads lenToRead number of bytes from sock, throws if wrong number of bytes
//    received

void readAndThrow(C150StreamSocket *sock, char *buf, ssize_t lenToRead) {
    if (readAndCheck(sock, buf, lenToRead) != success) {
        stringstream ss;
        ss << "rpcutils.readAndCheck: " << lenToRead << " bytes could not be "
           << "read";
        throw RPCException(ss.str());
    }
}


// checkArgs
//  - to be called after all arguments have been extracted from ss
//  - checks if there were too few or too many arguments byte-wise
//
//  note:
//  - whether or not arg bytes were correctly organized is NOT checked. this can
//    only be checked during arg parsing

StatusCode checkArgs(stringstream &ss) {
    if (ss.rdbuf()->in_avail() != 0) { // too many bytes even though args filled
        return too_few_args;
    } else if (ss.eof() && ss.fail()) { // too few bytes, args not fulfilled
        return too_many_args;
    } else {
        return good_args;
    }
}


// extractString
//  - extracts a string from a stringstream
//  - getline is not sufficient as the delimiter is not enforced on eof
//  - if no null term found, extractred chars are NOT putback into the stream
//    and exception is thrown
//
//  returns:
//      - s, extracted string from ss, since null term found

string extractString(stringstream &ss) {
    string s;
    char c;

    do {
        ss >> c;
        if (c == EOF) { // premature end
            throw RPCException(
                "rpcutils.extractString: Null terminator not found"
            );
        } else { // good 
            s += c;
        }
    } while (c != '\0');

    return s;
}


void print_bytes(const void *object, size_t size)
{
  // This is for C++; in C just drop the static_cast<>() and assign.
  const unsigned char * const bytes = static_cast<const unsigned char *>(object);
  size_t i;

  printf("[ ");
  for(i = 0; i < size; i++)
  {
    printf("%02x ", bytes[i]);
  }
  printf("]\n");
}
