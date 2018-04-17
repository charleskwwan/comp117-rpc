# shared.py
#
# Defines functions to generate c++ code that is shared between proxies and
# stubs
#
# by: Justin Jo and Charles Wan

import utils


# constants
SHARED_HEADERS = [
    '<cstdio>',
    '<cstring>',
    '<string>',
    '<sstream>',
    '"c150grading.h"',
    '"c150debug.h"',
    '"rpcutils.h"',
]
SHARED_NAMESPACES = [
    'std',
    'C150NETWORK',
]

# generate_incls
#   - generates c++ includes along with namespaces
#
#   args:
#   - headers [list[str]]: list of headers to be included
#       - headers should be of the form: <...> or "..."
#   - namespaces [list[str]]: list of namespaces
#
#   returns [str]: c++ includes and namespaces

def generate_incls(headers, namespaces):
    return '{}\n\n{}\n'.format(
        '\n'.join(['#include ' + h for h in headers]), # includes
        '\n'.join(['using namespace ' + n + ';' for n in namespaces]), # nspaces
    )


# generate_varhandle
#   - for each variable, add a read or write
#   - for simple types like int/string/float, only 1 r/w is necessary
#   - for array types, for loop to r/w 1 per element
#   - for structs, we do 1 r/w per member
#   - if arg types are nested, we nest the number of reads, according to the
#     above
#
# args:
#   - varname [str]: name of variable
#   - vartype [str]: type of variable, should have an entry in typedict
#   - typesdict [dict]: dictionary of types
#   - builtin_formats [dict]: format strings for r/w for builtin types
#       - must have entries for 'int', 'float' and 'string'
#   - n [int]: number of recursive calls so far, initialized to 0, should not be
#       used by nonrecursive calls
#       - needed to guarantee that iterators in for loops are unique
#
# returns [str]: c++ string of var r/w, or None if invalid type found

def generate_varhandle(varname, vartype, typesdict, builtin_formats, n=0):
    if vartype in typesdict:
        type_of_type = typesdict[vartype]['type_of_type']
    else:
        return None # type not found, invalid

    if type_of_type == 'builtin':
        if vartype in builtin_formats:
            return builtin_formats[vartype].format(varname)
        else:
            return None

    elif type_of_type == 'array':
        # for each level of array, generate a for loop
        iterator = 'i' + str(n)
        arrstr = utils.generate_forloop(
            iterator,
            0, typesdict[vartype]['element_count'],
        ) + '{\n'

        arrstr += generate_varhandle(
            varname + '[' + iterator + ']', # including indexing syntax
            typesdict[vartype]['member_type'], # element type update
            typesdict, builtin_formats, n + 1,
        )

        return arrstr + '}\n'

    elif type_of_type == 'struct':
        # iterate over each member of the struct and recursively call
        return ''.join([
            generate_varhandle(
                varname + '.' + p['name'], # include struct member access
                p['type'], # member type update
                typesdict, builtin_formats, n + 1,
            )
            for p in typesdict[vartype]['members']
        ])

    else: # invalid type of type found, bug
        return None


# generate debug lines for varreads and varwrites below
def _generate_rw_debug(ty, is_stub, is_read):
    distobj = 'stub' if is_stub else 'proxy'
    rw = 'Received' if is_read else 'Sending'

    return '\n'.join([
        'debugStream << "{}: {} {} \'" << {{0}} << "\' for variable \'{{0}}\'";'
            .format(distobj, rw, ty),
        'logDebug(debugStream, VARDEBUG, true);\n',
    ])


# generate_varreads
#   - for each variable, add the necessary number of reads to fill the variable
#     from a string stream

# args:
#   - varname [str]: name of variable
#   - vartype [str]: type of variable, should have an entry in typedict
#   - typesdict [dict]: dictionary of types
#   - is_stub [bool]: whether or not code is for the stub
#   - streamvar [str]: name of the stream 
#
# returns [str]: c++ string of var reads, or None if invalid type found
#
# notes:
#   - assumes existence of string stream 'debugStream' for debugging

def generate_varreads(varname, vartype, typesdict, is_stub, streamvar='ss'):
    builtin_formats = {
        'int': '{0} = extractInt(' + streamvar + ');\n',
        'float': '{0} = extractFloat(' + streamvar + ');\n',
        'string': '{0} = extractString(' + streamvar + ');\n',
    }
    for ty in builtin_formats.keys(): # append debug strings
        builtin_formats[ty] += _generate_rw_debug(ty, is_stub, True)

    return generate_varhandle(varname, vartype, typesdict, builtin_formats)


# generate_varwrites
#   - for each var, add writes to write entire variable to sock
#
# args:
#   - varname [str]: name of variable
#   - vartype [str]: type of variable, should have an entry in typedict
#   - typesdict [dict]: dictionary of types
#   - is_stub [bool]: whether or not code is for the stub
#
# returns [str]: c++ string of var reads, or None if invalid type found

def generate_varwrites(varname, vartype, typesdict, is_stub):
    sock = 'RPC' + ('STUB' if is_stub else 'PROXY') + 'SOCKET'

    builtin_formats = {
        'int': 'writeInt(' + sock + ', {0});\n',
        'float': 'writeFloat(' + sock + ', {0});\n',
        'string': 'writeAndCheck(' + sock + ', {0}.c_str(), {0}.length() + 1);\n',
    }
    for ty in builtin_formats.keys(): # prepend debug strings
        debugstr = _generate_rw_debug(ty, is_stub, False)
        builtin_formats[ty] = debugstr + builtin_formats[ty]

    return generate_varhandle(varname, vartype, typesdict, builtin_formats)


# generate_varsize
#   - generates c++ code to calculate the size of a variable
#
#   args:
#   - varname [str]: name of variable
#   - vartype [str]: type of variable, should have an entry in typedict
#   - typesdict [dict]: dictionary of types
#   - argsvar [str]: name of size argument

def generate_varsize(varname, vartype, typesdict, sizevar='sizeVar'):
    builtin_formats = {
        'int': sizevar + ' += 4;\n',
        'float': sizevar + ' += 4;\n',
        'string': sizevar + ' += {0}.length() + 1;\n', # +1 for null
    }
    return generate_varhandle(varname, vartype, typesdict, builtin_formats)
