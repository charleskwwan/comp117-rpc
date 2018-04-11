# proxy.py
#
# Defines functions to generate proxy specific code for rpcgenerate
#
# by: Justin Jo and Charles Wan

import shared
import utils


# generate_funcproxy
#   - generates the proxy for a c++ function in an idl file
#
#   args:
#   - funcname [str]: name of function
#   - funcsdict [dict]: idl func declarations in json
#   - typesdict [dict]: idl type declarations in json

def generate_funcproxy(funcname, funcsdict, typesdict):
    funcdict = funcsdict[funcname]
    returntype = funcdict['return_type']

    # generate calls to write values to socket
    argstrs = [
        shared.generate_varwrites(p['name'], p['type'], typesdict, False)
        for p in funcdict['arguments']
    ]

    # generate resultcode handling
    rescodestr = '\n'.join([
        'ResultCode code = readResultCode(RPCPROXYSOCKET);',
        'if (code != success) {',
        '  stringstream ss;',
        '  ss << "{}: received error result code " << code;',
        '  throw C150Exception(ss.str());',
        '}\n',
    ])

    # generate calls to read values to result for return

    return '\n'.join([
        utils.generate_funcheader(funcname, funcdict) + '{',
        'string funcname = "{}";'.format(funcname),
        'writeString(RPCPROXYSOCKET, funcname);\n',
        '\n'.join(argstrs),
        rescodestr,
        utils.generate_vardecl(returntype, 'res') + ';',
        shared.generate_varreads('res', returntype, typesdict, False),
        'return res;',
        '}\n',
    ])