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

writeInt(RPCPROXYSOCKET, funcnamelen);
RPCPROXYSOCKET->write(funcname.c_str(), funcnamelen);

// read funcname status code - does server know about this func?
StatusCode funcnameCode = (StatusCode)readInt(RPCPROXYSOCKET);;
if (funcnameCode != existing_func) {{
  debugStream << "proxy.{funcname}: " <<  debugStatusCode(funcnameCode);
  c150debug->printf(C150APPLICATION, debugStream.str().c_str());
  throw RPCException(debugStream.str().c_str());
}}

// send total size of all args
int argsSize = 0;
{argsSizeAccumulate}
writeInt(RPCPROXYSOCKET, argsSize);

// send args one by one
debugStream << "Sending arguments for {funcname}()";
logDebug(debugStream, C150APPLICATION, true);

{sendArgs}

// read args status code - does server like args?
StatusCode argsCode = (StatusCode)readInt(RPCPROXYSOCKET);
if (argsCode != good_args) {{
  debugStream << "proxy.{funcname}: " <<  debugStatusCode(funcnameCode);
  c150debug->printf(C150APPLICATION, debugStream.str().c_str());
  throw RPCException(debugStream.str().c_str());
}}

{% begin result %}
// read result size and result bytes
debugStream << "Receiving result for {funcname}()";
logDebug(debugStream, C150APPLICATION, true);

int resSize = readInt(RPCPROXYSOCKET);
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
