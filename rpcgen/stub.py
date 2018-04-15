# stub.py
#
# Defines functions to generate stub specific code for rpcgenerate
#
# by: Justin Jo and Charles Wan

import shared
import utils


# generate_funcstub
#   - generates the stub for a c++ function specified in an idl file
#
#   args:
#   - funcname [str]: name of function
#   - funcsdict [dict]: idl func declarations in json
#   - typesdict [dict]: idl type declarations in json
#
#   NEEDSWORK: use hdr arg to func stub to verify argument sizes

def generate_funcstub(funcname, funcsdict, typesdict):
    funcdict = funcsdict[funcname]
    args = funcdict['arguments']
    returntype = funcdict['return_type']
    stubstrs = []

    # func signature
    stubstrs.append('void _{}() {{'.format(funcname))

    # read args size and bytes
    stubstrs.extend([
        '// read args size then all arg bytes',
        'int argsSize;',
        'readAndThrow(RPCSTUBSOCKET, (char *)&argsSize, 4);',
        'char *argsBytes = new char[argsSize];',
        'readAndThrow(RPCSTUBSOCKET, argsBytes, argsSize);\n',
    ])

    # read args from string stream
    stubstrs.extend([
        '// use string stream to deconstruct args bytes into args',
        'stringstream ss;',
        'ss << string(argsBytes, argsSize);',
        'delete [] argsBytes;',
        'StatusCode argsCode = good_args;\n',
    ])
    stubstrs.append('\n'.join([ # var decls before try for scoping reasons
        utils.generate_vardecl(p['type'], p['name']) + ';\n'
        for p in args
    ]))
    stubstrs.append('try {')
    stubstrs.append('\n'.join([
        shared.generate_varreads(p['name'], p['type'], typesdict, True, 'ss')
        for p in args
    ]))
    stubstrs.extend([
        'argsCode = checkArgs(ss);',
        '} catch (RPCException e) {',
        '  argsCode = scrambled_args; // string not null terminated',
        '}\n',
    ])

    # send args code
    stubstrs.extend([
        'RPCSTUBSOCKET->write((char *)&argsCode, 4);',
        'if (argsCode != good_args) {',
        '  c150debug->printf(',
        '    C150APPLICATION,',
        '    "stub.{}: Error %d occurred when parsing args",'
        '    argsCode',
        '  );',
        '}\n',
    ])

    # call func with args
    stubstrs.append('{} = {}({});\n'.format(
        utils.generate_vardecl(returntype, 'res'), # result
        funcname,
        ', '.join(p['name'] for p in args),
    ))

    # send result size then result
    stubstrs.extend([
        '// send total size of result',
        'int resSize = 0;',
        shared.generate_varsize('res', returntype, typesdict, 'resSize'),
        'RPCSTUBSOCKET->write((char *)&resSize, 4);\n',
        shared.generate_varwrites('res', returntype, typesdict, True),
    ])

    # func termination
    stubstrs.append('}\n')

    return '\n'.join(stubstrs)


# generate_dispatch
#   - generates c++ function that dispatches function requests received from the
#     socket
#
#   args:
#   - funcsdict [dict]: idl func declarations in json
#   - prefix [str]: prefix of idl file

def generate_dispatch(funcsdict, prefix):
    dispatchstrs = []

    # function signature
    dispatchstrs.extend([
        'void dispatchFunction() {',
        'try {',
    ])

    # read func size then name
    dispatchstrs.extend([
        '// read func name length then its name',
        'int funcnamelen;',
        'readAndThrow(RPCSTUBSOCKET, (char *)&funcnamelen, 4);',
        'char *funcname = new char[funcnamelen];',
        'readAndThrow(RPCSTUBSOCKET, funcname, funcnamelen);\n',
        "if (funcname[funcnamelen-1] != '\\0') {",
        '  throw RPCException("dispatchFunction: Function name not null terminated");',
        '}\n',
    ])

    # branch to check funcname validity
    branch_format = '\n'.join([
        'if (strcmp(funcname, "{0}") == 0) {{',
        '  StatusCode funcnameCode = existing_func;',
        '  RPCSTUBSOCKET->write((char *)&funcnameCode, 4);',
        '  _{0}();',
        '}}',
    ])
    dispatchstrs.extend([
        '// check to see if funcname is valid, and dispatch if so',
        'if (!RPCSTUBSOCKET->eof()) {',
        ' else '.join([
            branch_format.format(f)
            for f in funcsdict.keys()
        ]) + ' else {',
        '  StatusCode funcnameCode = nonexistent_func;',
        '  RPCSTUBSOCKET->write((char *)&funcnameCode, 4);',
        '}',
        '}\n',
        
    ])

    # function termination
    dispatchstrs.extend([
        '} catch (RPCException e) {',
        '  c150debug->printf(',
        '    C150APPLICATION,',
        '    "Caught %s",',
        '    e.formattedExplanation().c_str()',
        '  );',
        '}',
        '}\n',
    ])

    return '\n'.join(dispatchstrs)
