// --------------------------------------------------------------
//
//                        rpcserver.cpp
//
//        Author: Noah Mendelsohn         
//   
//
//        This is a skeleton of a server program for RPC.
//        Its quite generic. You may modify it if you need to,
//        but it might also be usable in its current form.
//
//        Note that you may want to use this as the main
//        program for many different executables, each of which
//        you link with a different specific stub, and save
//        under a different executable name. So:
//
//             g++ rpcgameserver rpcserver.cpp gamestub.o
//             g++ rpcstockquoteserver rpcserver.cpp stockquotestub.o
//      
//
//        This program loops accepting connections on a stream socket.
//        On each connection it loops calling the main stub 
//        entry "dispatchFunction()" to invoke one function for
//        the remote client.
//
//        COMMAND LINE
//
//              <whatevernameyoulinkthis as>
//
//        OPERATION
//
//
//       Copyright: 2012 Noah Mendelsohn
//
//       Modified by: Justin Jo and Charles Wan
//     
// --------------------------------------------------------------

// define debug file, can be set by compiler
#ifndef _DEBUG_FILE_
#define _DEBUG_FILE_ NULL
#endif

#include "rpcstubhelper.h"

#include "c150debug.h"
#include "c150grading.h"
#include <fstream>
#include <sstream>
#include "rpcutils.h"

using namespace std;          // for C++ std library
using namespace C150NETWORK;  // for all the comp150 utilities 


// fwd declarations
void usage(char *progname, int exitCode);


// cmd line args
const int numberOfArgs = 0;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                           main program
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
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
        // set up socket
        rpcstubinitialize();

        while (1) {
            // wait for client to connect
            c150debug->printf(
                C150RPCDEBUG,
                "rpcserver: calling C150StreamSocket::accept"
            );
            RPCSTUBSOCKET->accept();

            // infinite message processing
            while (1) {
                dispatchFunction();

                if (RPCSTUBSOCKET->eof()) {
                    c150debug->printf(
                        C150RPCDEBUG,
                        "rpcserver: EOF signaled on input"
                    );
                    break;
                }
            }

            // close current, wait for next client
            c150debug->printf(C150RPCDEBUG,"Calling C150StreamSocket::close");
            RPCSTUBSOCKET->close();
        }

    } catch (C150Exception e) {
        // write to debug log
        c150debug->printf(
            C150ALWAYSLOG,
            "Caught %s",
            e.formattedExplanation().c_str()
        );
        // in case logging to file, write to console too
        cerr << argv[0] << ": " << e.formattedExplanation() << endl; 
    }

    RPCSTUBSOCKET->close(); // just in case
    return 0;
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
    fprintf(stderr, "usage: %s\n", progname);
    exit(exitCode);
}
