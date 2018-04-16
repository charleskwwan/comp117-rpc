// funcproxy.template.cpp
//
// Defines a template for an rpc proxy function to be filled in by rpcgenerate
//  - leaves Python format strings for where things should be filled out
//    - e.g. {funcname} 
//
// by: Justin Jo and Charles Wan

{funcheader} {{
stringstream debugStream;

//send funcname length then funcname
string funcname = "{funcname}";
int funcnamelen = funcname.length() + 1; // +1 for null terminator

debugStream << "Requesting to call {funcname}()"; // log func request
logDebug(debugStream, C150APPLICATION, true);

RPCPROXYSOCKET->write((char *)&funcnamelen, 4);
RPCPROXYSOCKET->write(funcname.c_str(), funcnamelen);

// read funcname status code - does server know about this func?
StatusCode funcnameCode;
readAndThrow(RPCPROXYSOCKET, (char *)&funcnameCode, 4);
if (funcnameCode != existing_func) {{
  debugStream << "proxy.{funcname}: " <<  debugStatusCode(funcnameCode);
  c150debug->printf(C150APPLICATION, debugStream.str().c_str());
  throw RPCException(debugStream.str().c_str());
}}

// send total size of all args
int argsSize = 0;
{argsSizeAccumulate}
RPCPROXYSOCKET->write((char *)&argsSize, 4);

// send args one by one
debugStream << "Sending arguments for {funcname}()";
logDebug(debugStream, C150APPLICATION, true);

{sendArgs}

// read args status code - does server like args?
StatusCode argsCode;
readAndThrow(RPCPROXYSOCKET, (char *)&argsCode, 4);
if (argsCode != good_args) {{
  debugStream << "proxy.{funcname}: " <<  debugStatusCode(funcnameCode);
  c150debug->printf(C150APPLICATION, debugStream.str().c_str());
  throw RPCException(debugStream.str().c_str());
}}

{% begin result %}
// read result size and result bytes
int resSize;
readAndThrow(RPCPROXYSOCKET, (char *)&resSize, 4);

debugStream << "Receiving result for {funcname}()";
logDebug(debugStream, C150APPLICATION, true);

char resBytes[resSize];
readAndThrow(RPCPROXYSOCKET, resBytes, resSize);

// use string stream to deconstruct result bytes into result for return
stringstream ss;
ss << string(resBytes, resSize);
{declareResult} // result must be named res
{readResult}

return res;
{% end result %}
}}
