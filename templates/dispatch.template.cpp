// funcproxy.template.cpp
//
// Defines a template for dispatchFunction to be filled in by rpcgenerate
//  - dispatchFunction is stub exclusive
//  - leaves Python format strings for where things should be filled out
//    - e.g. {funcname} 
//
// by: Justin Jo and Charles Wan

void dispatchFunction() {{
stringstream debugStream;

try {{
// read funcname length then name
int funcnamelen = readInt(RPCSTUBSOCKET);
char funcname[funcnamelen];
readAndThrow(RPCSTUBSOCKET, funcname, funcnamelen);

if (funcname[funcnamelen - 1] != '\0') {{ // check funcname null termed
  writeInt(RPCSTUBSOCKET, no_null_term_found);
  throw RPCException("dispatchFunction: Function name not null terminated");
}}

// debug for func request
debugStream << "Received function request for " << funcname << "()";
logDebug(debugStream, C150APPLICATION, true);

// branch to check funcname validity
if (!RPCSTUBSOCKET->eof()) {{
{funcBranches} else {{
  // nonexisting function requested
  debugStream << "Unknown function " << funcname << "() requested";
  logDebug(debugStream, C150APPLICATION, true);
  writeInt(RPCSTUBSOCKET, nonexistent_func);
}}
}}

debugStream << "Function request for " << funcname << "() complete";
logDebug(debugStream, C150APPLICATION, true);
}} catch (RPCException e) {{
  // something went wrong with dispatch, but shouldnt end server
  c150debug->printf(C150APPLICATION,
    "Caught %s",
    e.formattedExplanation().c_str());
}}
}}
