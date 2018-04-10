# utils.py
#
# Defines utility functions for rpcgenerate
#
# by: Justin Jo and Charles Wan

import os
import re


# isfile
#   - returns [bool]:
#       - true, if file fname exists and is readable
#       - false, otherwise

def isfile(fname):
    return os.path.isfile(fname) and os.access(fname, os.R_OK)


# get_idl_prefix
#   - for an idl file <idlname>.idl, extracts and returns idlname
#   - if idlfile does not end with .idl, it is assumed not to be an idl file and
#     None is returned

def get_file_prefix(idlfile):
    m = re.search('(.+).idl', idlfile)
    return m.group(1) if m else None


# clean_type
#   - cleans a type from idl's json format
#   - cleaned:
#       - leading '__', for arrays

def clean_type(ty):
    m = re.search('__(\S+)', ty)
    return m.groups()[0] if m else ty


# generate_var_decl
#   - generates a c++ variable declaration 
#   - decl format: vartype varname([digit]*)
#
#   notes:
#   - terminating ; is not included
#   - array types have the form: type([digit]+)

def generate_vardecl(vartype, varname):
    vartype = clean_type(vartype)

    if '[' in vartype and ']' in vartype: # array type
        # move indexing parts of type to name
        m = re.search('([^\[\]]+)([\[\]0-9]+)', vartype)
        vartype = m.groups()[0]
        varname += m.groups()[1]

    return vartype + ' ' + varname


# generate_funcheader
#   - generates a header for a c++ function
#   - header format: 'returntype funcname (argtype argnamem, ...)'
#
#   args:
#   - funcname [str]: name of function
#   - funcdict [dict]: json dict containing return type and args for funcname
#
#   notes:
#   - curly braces are not included

def generate_funcheader(funcname, funcdict):
    returntype = clean_type(funcdict['return_type'])    
    args = [ # iterate over pairs of arguments, organize into list of arg strs
        (clean_type(p['type']) + ' ' + p['name'])
        for p in funcdict['arguments']
    ]
    return returntype + ' ' + funcname + ' (' + ', '.join(args) + ')'


# generate_funccall
#   - generates a function call for a c++ function
#   - call format: 'funcname(arg1, arg2, ...)'
#
#   args:
#   - funcname [str]: name of function
#   - funcargs [list[str]]: list of arguments to be passed into the function
#
#   notes:
#   - terminating ; not included

def generate_funccall(funcname, funcargs):
    return '{}({})'.format(
        funcname,
        ', '.join(funcargs)
    )
