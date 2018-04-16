// funcproxy.template.cpp
//
// Defines a template for an rpc proxy function to be filled in by rpcgenerate
//  - leaves Python format strings for where things should be filled out
//
// by: Justin Jo and Charles Wan

{funcheader} {{
stringstream debugStream;

//send funcname length then funcname
string funcname = "{funcname}";
int funcnamelen = funcname.length() + 1; // +1 for null terminator
RPCPROXYSOCKET->write((char *)&funcnamelen, 4);
RPCPROXYSOCKET->write(funcname.c_str(), funcnamelen);

// read funcname status code - does server know about this func?
StatusCode funcnameCode;
readAndThrow(RPCPROXYSOCKET, (char *)&funcnameCode, 4);
if (funcnameCode != existing_func) {{
  throw RPCException("proxy.{funcname}: Server could not verify function");
}}

// send total size of al largs
int argsSize = 0;
{argsSizeAccumulate}
RPCPROXYSOCKET->write((char *)&argsSize, 4);

// send args one by one
{sendArgs}

// read args status code - does server like args?
StatusCode argsCode;
readAndThrow(RPCPROXYSOCKET, (char *)&argsCode, 4);
if (argsCode != good_args) {{
  throw RPCException("proxy.{funcname}: Server could not verify args");
}}

// read result size and result bytes
int resSize;
readAndThrow(RPCPROXYSOCKET, (char *)&resSize, 4);
char resBytes[resSize];
readAndThrow(RPCPROXYSOCKET, resBytes, resSize);

// use string stream to deconstruct result bytes into result for return
stringstream ss;
ss << string(resBytes, resSize);
{declareResult} // result must be named res
{readResult}

return res;
}}
