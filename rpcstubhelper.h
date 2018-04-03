// --------------------------------------------------------------
//
//                        rpcstubhelper.h
//
//        Author: Noah Mendelsohn         
//   
//
//        This file provides a helper routine to open a socket and put
//        it in a global variable where individual stub routines can find it.
//
//        OPERATION
//
//        Call rpcstubinitialize() to open the socket.
//        If there's a problem an exception will be thrown.
//
//        After calling rpcstubinitialize, call 
//        RPCSTUBSOCKET->accept() to accept a new connection,
//        RPCSTUBSOCKET-> read/write
//        when RPCSTUBSOCKET->eof goes true, then 
//        RPCSTUBSOCKET->closerpcstubaccept
//        to wait for a new incoming connection
//
//        All the operations listed above can be repeated from accept
//        to close, to process new connections.
//
//        LIMITATIONS
//
//              This version does not timeout 
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef __RPCSTUBHELPER_H_INCLUDED__  
#define __RPCSTUBHELPER_H_INCLUDED__  



#include "c150streamsocket.h"
#include "c150debug.h"


using namespace C150NETWORK;  // for all the comp150 utilities 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//    Global variable where stubs can find socket.
//
//    Note: the framework uses the same object to listen and
//    read/write. You can do accept again after closing.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

extern C150StreamSocket *RPCSTUBSOCKET;
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                rpcstubinitialize
//
//     Opens the socket and leaves it in global variable.
//     Note that the socket call may throw an exception 
//     which is NOT caught here.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void
rpcstubinitialize();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                dispatchFunction
//
//     This function is actually in each of the particular
//     generated stubs, but this is a common place to
//     declare it where rpcserver will see it.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void dispatchFunction();

#endif
