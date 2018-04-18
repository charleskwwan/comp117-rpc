# utils.py
#
# Defines utility functions for rpcgenerate
#
# by: Justin Jo and Charles Wan

import os
import re


TEMPLATES_DIR = './templates/'


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
    m = re.match(r'(.+/)*(.+)\.idl', idlfile)
    return m.group(2) if m else None


# clean_type
#   - cleans a type from idl's json format
#   - cleaned:
#       - leading '__', for arrays

def clean_type(ty):
    m = re.search('__(\S+)', ty)
    return m.groups()[0] if m else ty


# load_template
#   - loads a returns a cpp template from file as a string
#   - cleans leading comments

def load_template(tmpfile):
    with open(TEMPLATES_DIR + tmpfile, 'r') as f:
        template = f.read()

    m = re.match(r'(//.*\n)*\n', template) # remove leading c++ comments
    return template[len(m.group()):] if m else template


# replace_template_block
#   - replaces the FIRST block named blockname in a given template
#   - blocks are delimited by:
#       - begin: {% begin <blockname> %}
#       - end: {% end <blockname> %}
#
#   args:
#   - template [str]: loaded template string
#   - blockname [str]: name of block, see above for how it is used
#   - repl [str]: string to replace block with 
#       - if None, keep the block but remove the tags
#
#   returns [str]: template with block replaced

def replace_template_block(template, blockname, repl=None):   
    if repl != None: # allow repl == '', which is deletion completely
        # if replacement specified, just replace fully
        return re.sub(
            r'\{{% begin {0} %\}}\n{{0,1}}[\s\S]*\{{% end {0} %\}}'.format(blockname),
            repl, template
        )
    
    # if replacement specified, get rid of block specifiers
    m = re.search(
        r'\{{% begin {0} %\}}\n{{0,1}}([\s\S]*)\{{% end {0} %\}}\n*'
            .format(blockname),
        template,
    )
    return template.replace(m.group(), m.groups()[0]) if m else template


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
        generate_vardecl(p['type'], p['name'])
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


# generate_forloop
#   - generates the first line of a c++ for loop
#   - assumes a for loop that increments an iterator by one each iteration until
#     a given maximum
#
#   args:
#   - iterator [str]: iterator variable name
#   - lo [int]: starting value of iteratr
#   - hi [int]: final value of iterator
#
#   notes:
#   - curly braces not included

def generate_forloop(iterator, lo, hi):
    return 'for (int {0} = {1}; {0} < {2}; {0}++)'.format(iterator, lo, hi)
