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

    template_formats = {
        'funcname': funcname,
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
