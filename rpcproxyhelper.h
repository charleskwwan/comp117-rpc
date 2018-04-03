// --------------------------------------------------------------
//
//                        rpcproxyhelper.h
//
//        Author: Noah Mendelsohn         
//   
//
//        This file provides a helper routine to open a socket and pu
//        it in a global variable where individual proxy routines can find it.
//
//        OPERATION
//
//        Call rpcproxyinitialize(servername) to open the socket.
//        If there's a problem an exception will be thrown.
//
//        LIMITATIONS
//
//              This version does not timeout 
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#ifndef __RPCPROXYHELPER_H_INCLUDED__  
#define __RPCPROXYHELPER_H_INCLUDED__  

#include "c150streamsocket.h"
#include "c150debug.h"
// #include <fstream>

using namespace C150NETWORK;  // for all the comp150 utilities 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//    Global variable where proxies can find socket.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

extern C150StreamSocket *RPCPROXYSOCKET;
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                rpcproxyinitialize
//
//     Opens the socket and leaves it in global variable.
//     Note that the socket call may throw an exception 
//     which is NOT caught here.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void rpcproxyinitialize(char *servername);

#endif
