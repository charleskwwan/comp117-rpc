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
// --------------------------------------------------------------



#include "rpcstubhelper.h"

#include "c150debug.h"
#include "c150grading.h"
#include <fstream>
#include <sstream>

using namespace std;          // for C++ std library
using namespace C150NETWORK;  // for all the comp150 utilities 

// forward declarations
void setUpDebugLogging(const char *logname, int argc, char *argv[]);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                           main program
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
int 
main(int argc, char *argv[]) {

     //
     //  Set up debug message logging
     //
     stringstream logfilename;
     logfilename << argv[0] << "debug.txt"; 
     setUpDebugLogging(logfilename.str().c_str(),argc, argv);

     //
     // Make sure command line looks right
     //
     if (argc != 1) {
       fprintf(stderr,"Correct syntxt is: %s ... (there are no arguments) \n", argv[0]);
       exit(1);
     }
     //
     //  DO THIS FIRST OR YOUR ASSIGNMENT WON'T BE GRADED!
     //
     
     GRADEME(argc, argv);

     //
     //     Call the functions and see if they return
     //
     try {

       //
       // Set up the socket so the stubs can find it
       //
       rpcstubinitialize();


       //
       // Infinite loop accepting connections
       //
       while (1) {

	 //
	 // We'll hang here until another client asks to connect
	 // When we drop through, RPCSTUBSOCKET will talk to that one
	 // Note that RPCSTUBSOCKET is a global variable set up by
         // rpcstubinitialize and declared in rpcstubhelper.h
	 //
	 c150debug->printf(C150RPCDEBUG,"rpcserver.cpp:"
                                        "calling C150StreamSocket::accept");
         RPCSTUBSOCKET -> accept();
	 
       
         //
         // infinite loop processing messages
         //
         while(1)	{
  
  	   //
	   // Call a function for the client. The stubs will do the 
	   // work of reading and writing the stream, but will return on eof,
	   // which we'll get when client goes away.
	   //
	   dispatchFunction();     // call the subs function dispatcher
  
	   if (RPCSTUBSOCKET -> eof()) {
  	       c150debug->printf(C150RPCDEBUG,"rpcserver.cpp: EOF signaled on input");
	       break;
	   }
	 }                         // end: while processing messages

	 //
	 // Done looping on this connection, close and wait for another
	 //
         c150debug->printf(C150RPCDEBUG,"Calling C150StreamSocket::close");
	 RPCSTUBSOCKET -> close();
       }                          // end: while processing all connections
     }

     //
     //  Handle networking errors -- for now, just print message and give up!
     //
     catch (C150Exception e) {
       // Write to debug log
       c150debug->printf(C150ALWAYSLOG,"Caught C150Exception: %s\n",
			 e.formattedExplanation().c_str());
       // In case we're logging to a file, write to the console too
       cerr << argv[0] << ": caught C150NetworkException: " << e.formattedExplanation() << endl;
     }

     return 0;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                     setUpDebugLogging
//
//        For COMP 150-IDS, a set of standards utilities
//        are provided for logging timestamped debug messages.
//        You can use them to write your own messages, but 
//        more importantly, the communication libraries provided
//        to you will write into the same logs.
//
//        As shown below, you can use the enableLogging
//        method to choose which classes of messages will show up:
//        You may want to turn on a lot for some debugging, then
//        turn off some when it gets too noisy and your core code is
//        working. You can also make up and use your own flags
//        to create different classes of debug output within your
//        application code
//
//        NEEDSWORK: should be factored into shared code w/pingstreamserver
//        NEEDSWORK: document arguments
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
void setUpDebugLogging(const char *logname, int argc, char *argv[]) {

     //   
     //           Choose where debug output should go
     //
     // The default is that debug output goes to cerr.
     //
     // Uncomment the following three lines to direct
     // debug output to a file. Comment them
     // to default to the console.
     //
     // Note: the new DebugStream and ofstream MUST live after we return
     // from setUpDebugLogging, so we have to allocate
     // them dynamically.
     //
     //
     // Explanation: 
     // 
     //     The first line is ordinary C++ to open a file
     //     as an output stream.
     //
     //     The second line wraps that will all the services
     //     of a comp 150-IDS debug stream, and names that filestreamp.
     //
     //     The third line replaces the global variable c150debug
     //     and sets it to point to the new debugstream. Since c150debug
     //     is what all the c150 debug routines use to find the debug stream,
     //     you've now effectively overridden the default.
     //
     ofstream *outstreamp = new ofstream(logname);
     DebugStream *filestreamp = new DebugStream(outstreamp);
     DebugStream::setDefaultLogger(filestreamp);

     //
     //  Put the program name and a timestamp on each line of the debug log.
     //
     c150debug->setPrefix(argv[0]);
     c150debug->enableTimestamp(); 

     //
     // Ask to receive all classes of debug message
     //
     // See c150debug.h for other classes you can enable. To get more than
     // one class, you can or (|) the flags together and pass the combined
     // mask to c150debug -> enableLogging 
     //
     // By the way, the default is to disable all output except for
     // messages written with the C150ALWAYSLOG flag. Those are typically
     // used only for things like fatal errors. So, the default is
     // for the system to run quietly without producing debug output.
     //
     c150debug->enableLogging(C150RPCDEBUG | C150APPLICATION | C150NETWORKTRAFFIC | 
			      C150NETWORKDELIVERY); 
}
