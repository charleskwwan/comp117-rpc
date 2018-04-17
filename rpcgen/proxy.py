# proxy.py
#
# Defines functions to generate proxy specific code for rpcgenerate
#
# by: Justin Jo and Charles Wan

import shared
import utils


FUNCPROXY_TEMPLATE = 'funcproxy.template.cpp'


# generate_funcproxy
#   - generates the proxy for a c++ function in an idl file
#
#   args:
#   - funcname [str]: name of function
#   - funcsdict [dict]: idl func declarations in json
#   - typesdict [dict]: idl type declarations in json

def generate_funcproxy(funcname, funcsdict, typesdict):
    template = utils.load_template(FUNCPROXY_TEMPLATE)
    funcdict = funcsdict[funcname]
    args = funcdict['arguments']
    returntype = funcdict['return_type']

    # if no args, remove args block in template
    template = utils.replace_template_block(
        template, 'args',
        repl=('' if len(args) == 0 else None),
    )

    # if void, replace result block in template with just a return
    template = utils.replace_template_block(
        template, 'result',
        repl=('\nreturn;' if returntype == 'void' else None),
    )

    template_formats = {
        'funcname': funcname,
        'returntype': returntype,
        'funcheader': utils.generate_funcheader(funcname, funcdict),
        'argsSizeAccumulate': ''.join([
            shared.generate_varsize(p['name'], p['type'], typesdict, 'argsSize')
            for p in args
        ]),
        'sendArgs': '\n'.join([
            shared.generate_varwrites(p['name'], p['type'], typesdict, False)
            for p in args
        ]),
        'declareResult': utils.generate_vardecl(returntype, 'res') + ';',
        'readResult': shared.generate_varreads('res', returntype, typesdict, False, 'ss'),
    }
    return template.format(**template_formats)
