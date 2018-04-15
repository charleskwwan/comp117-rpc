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
    args = funcdict['arguments']
    returntype = funcdict['return_type']
    proxystrs = []

    # function signature
    proxystrs.append(utils.generate_funcheader(funcname, funcdict) + '{')

    # send funcname length then name
    proxystrs.extend([
        '// send funcname length then name',
        'string funcname = "{}";'.format(funcname),
        'int funcnamelen = {};'.format(len(funcname) + 1), # +1 for null
        'RPCPROXYSOCKET->write((char *)&funcnamelen, 4);',
        'RPCPROXYSOCKET->write(funcname.c_str(), funcnamelen);\n',
    ])

    # funcname status code
    proxystrs.extend([
        '// read funcname status code - does server know about this func?',
        'StatusCode funcnameCode;',
        'readAndThrow(RPCPROXYSOCKET, (char *)&funcnameCode, 4);',
        'if (funcnameCode != existing_func) {',
        '  throw C150Exception("{}: Server could not verify function");'
            .format(funcname),
        '}\n',
    ])

    # send arg size
    proxystrs.extend([
        '// send total size of all args',
        'int argsSize = 0;',
        '\n'.join([
            shared.generate_varsize(p['name'], p['type'], typesdict, 'argsSize')
            for p in args
        ]),
        'RPCPROXYSOCKET->write((char *)&argsSize, 4);\n',
    ])

    # send args
    proxystrs.append('// send args one by one')
    proxystrs.append('\n'.join([
        shared.generate_varwrites(p['name'], p['type'], typesdict, False)
        for p in args
    ]))

    # read args status code
    proxystrs.extend([
        '// read args status code - does server like args?',
        'StatusCode argsCode;',
        'readAndThrow(RPCPROXYSOCKET, (char *)&argsCode, 4);',
        'if (argsCode != good_args) {',
        '  throw C150Exception("{}: Server could not verify args");'
            .format(funcname),
        '}\n',
    ])

    # read res size and res bytes
    proxystrs.extend([
        '// read size of result, then all result bytes',
        'int resSize;',
        'readAndThrow(RPCPROXYSOCKET, (char *)&resSize, 4);',
        'char *resBytes = new char[resSize];',
        'readAndThrow(RPCPROXYSOCKET, resBytes, resSize);\n',
    ])

    # read res from string stream
    proxystrs.extend([
        '// use string stream to deconstruct result bytes into result',
        'stringstream ss;',
        'ss << string(resBytes, resSize);',
        'delete [] resBytes;',
        utils.generate_vardecl(returntype, 'res') + ';',
        shared.generate_varreads('res', returntype, typesdict, False, 'ss'),
        'return res;',
    ])

    # function termination
    proxystrs.append('}\n')

    return '\n'.join(proxystrs)
