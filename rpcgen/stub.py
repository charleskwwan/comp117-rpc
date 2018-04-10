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
    returntype = funcdict['return_type']

    # generate variable/argument declarations, followed by calls to read values
    # from socket
    argstrs = [
        # declaration \ read
        utils.generate_vardecl(p['type'], p['name']) + ';\n' +\
        shared.generate_varreads(p['name'], p['type'], typesdict, True)
        for p in funcdict['arguments']
    ]

    # generate line to make call to func
    callstr = '{} = {}({});'.format(
        utils.generate_vardecl(returntype, 'res'), # var to save
        funcname,
        ', '.join([p['name'] for p in funcdict['arguments']]) # args
    )

    return '\n'.join([
        'void {}(const FuncHeader &hdr) {{'.format(funcname),
        '\n'.join(argstrs), # arg declarations and reads
        callstr,
        'writeStatusCode(RPCSTUBCSOCKET, success);',
        shared.generate_varwrites('res', returntype, typesdict, True),
        '}\n',
    ])


# generate_dispatch
#   - generates c++ function that dispatches function requests received from the
#     socket
#
#   args:
#   - funcsdict [dict]: idl func declarations in json
#   - prefix [str]: prefix of idl file

def generate_dispatch(funcsdict, prefix):
    # for each, check name and number of supposd arguments, before calling
    # function's unique stub/wrapper
    funcstr = '\n    '.join([
        'if (hdr.funcname.compare("{0}") == 0 &&',
        '    hdr.argsSizes.sizes() == {1}) {{',
        '  _{0}(hdr);',
        '}}',
    ])
    func_branches = ' else '.join([
        funcstr.format(f, len(funcsdict[f]['arguments']))
        for f in funcsdict.keys()
    ])

    return '\n'.join([
        'void dispatchFunction() {',
        '  FuncHeader hdr;',
        '  readFuncHeader(RPCSTUBSOCKET, hdr);',
        '',
        '  if (!RPCSTUBSOCKET->eof()) {',
        '    ' + func_branches + ' else {',
        '      // flush stream of args to nonexistent function',
        '      for (size_t i = 0; i < hdr.argsSizes.size(); i++) {',
        '        readString(RPCSTUBSOCKET, hdr.argsSizes[i]);',
        '      }',
        '      writeStatusCode(RPCSTUBSOCKET, nonexistent_function);',
        '    }',
        '  }\n'
    ])
