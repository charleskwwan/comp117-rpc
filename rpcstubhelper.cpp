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

#include "rpcstubhelper.h"

using namespace C150NETWORK;  // for all the comp150 utilities 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//    Global variable where proxies can find socket.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

C150StreamSocket *RPCSTUBSOCKET;
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                rpcstubinitialize
//
//     Opens the socket and leaves it in global variable.
//     Put it in listening state.
//     Note that the socket call may throw an exception 
//     which is NOT caught here.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void rpcstubinitialize() {

  // create new socket
  c150debug->printf(C150RPCDEBUG,"rpcstubinitialize: Creating C150StreamSocket");
  RPCSTUBSOCKET = new C150StreamSocket();

  // Tell the OS to start allowing connections
  // The application will pick those up one at a time by doing accept calls
  RPCSTUBSOCKET -> listen();  
}
