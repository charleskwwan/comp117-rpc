// client.template.cpp
//
// Defines a standard template for building a client
//  - usage: ./<clientname> <server name>
//
// Labels with "INSERT HERE" should be replaced with the appropriate things
//
// by: Justin Jo and Charles Wan


// define debug file, can be set by compiler
#ifndef _DEBUG_FILE_
#define _DEBUG_FILE_ NULL
#endif

#include <string>
#include "rpcproxyhelper.h"
#include "c150debug.h"
#include "c150grading.h"
#include "rpcutils.h"

using namespace std;
using namespace C150NETWORK;

// INSERT HERE: idl file include, need string with namespace above to work


// fwd declarations
void usage(char *progname, int exitCode);


// cmd line args
const int numberOfArgs = 1;
const int serverArg = 1;


// ==========
// 
// MAIN
//
// ==========

int main(int argc, char *argv[]) {
    GRADEME(argc, argv); // obligatory grading line

    // cmd line handling
    if (argc != 1 + numberOfArgs) {
        usage(argv[0], 1);
    }

    // debugging
    uint32_t debugClasses = C150APPLICATION | C150RPCDEBUG | VARDEBUG;
    initDebugLog(_DEBUG_FILE_, argv[0], debugClasses);

    try {
        // create socket
        rpcproxyinitialize(argv[serverArg]);

        // INSERT HERE: call proxy functions

    } catch (C150Exception e) {
        // write to debug log
        c150debug->printf(
            C150ALWAYSLOG,
            "Caught %s",
            e.formattedExplanation().c_str()
        );
        // if logging to file, write to console too
        if (_DEBUG_FILE_ != NULL)
            cerr << argv[0] << ": " << e.formattedExplanation() << endl; 
    }
}


// ==========
// 
// DEFS
//
// ==========

// ==========
// GENERAL
// ==========

// Prints command line usage to stderr and exits
void usage(char *progname, int exitCode) {
    fprintf(stderr, "usage: %s <server>\n", progname);
    exit(exitCode);
}
