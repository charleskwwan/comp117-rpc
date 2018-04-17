# stub.py
#
# Defines functions to generate stub specific code for rpcgenerate
#
# by: Justin Jo and Charles Wan

import shared
import utils


# constants
FUNCSTUB_TEMPLATE = 'funcstub.template.cpp'
DISPATCH_TEMPLATE = 'dispatch.template.cpp'


# generate_funcstub
#   - generates the stub for a c++ function specified in an idl file
#
#   args:
#   - funcname [str]: name of function
#   - funcsdict [dict]: idl func declarations in json
#   - typesdict [dict]: idl type declarations in json

def generate_funcstub(funcname, funcsdict, typesdict):
    template = utils.load_template(FUNCSTUB_TEMPLATE)
    funcdict = funcsdict[funcname]
    args = funcdict['arguments']
    returntype = funcdict['return_type']

    # if no args remove args block in template
    template = utils.replace_template_block(
        template, 'args',
        repl=('' if len(args) == 0 else None),
    )

    # if void, replace return block in template with just a return
    template = utils.replace_template_block(
        template, 'result',
        repl=('\nreturn;' if returntype == 'void' else None),
    )

    template_formats = {
        'funcname': funcname,
        'returntype': returntype,
        'declareArgs': '\n'.join([
            utils.generate_vardecl(p['type'], p['name']) + ';'
            for p in args
        ]),
        'readArgs': '\n'.join([
            shared.generate_varreads(p['name'], p['type'], typesdict, True, 'ss')
            for p in args
        ]),
        'callFunction': '{}{}({});'.format(
            '' if returntype == 'void' else (utils.generate_vardecl(returntype, 'res') + ' = '),
            funcname,
            ', '.join(p['name'] for p in args),
        ),
        'resSizeAccumulate': shared.generate_varsize('res', returntype, typesdict, 'resSize'),
        'sendRes': shared.generate_varwrites('res', returntype, typesdict, True),
    }

    return template.format(**template_formats)


# generate_dispatch
#   - generates c++ function that dispatches function requests received from the
#     socket
#
#   args:
#   - funcsdict [dict]: idl func declarations in json
#   - prefix [str]: prefix of idl file

def generate_dispatch(funcsdict, prefix):
    template = utils.load_template(DISPATCH_TEMPLATE)

    template_formats = {
        'funcBranches': ' else '.join([
            '\n'.join([
                'if (strcmp(funcname, "{0}") == 0) {{',
                '  writeInt(RPCSTUBSOCKET, existing_func);',
                '  _{0}();',
                '}}',
            ]).format(f)
            for f in funcsdict.keys()
        ]),
    }
    return template.format(**template_formats)
