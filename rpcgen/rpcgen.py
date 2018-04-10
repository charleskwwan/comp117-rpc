#!/usr/bin/env python3

# rpcgen.py
#
# Defines functions to generate proxies and stubs for an idl file
#   - usage: rpcgenerate [-h] idlfiles [idlfiles ...]
#   - output: <name>.proxy.cpp, <name>.stub.cpp
#
# by: Justin Jo and Charles

import argparse

import utils
import shared


##### MISCELLANEOUS FUNCTIONS

# parse_args
#   - parse cmd line arguments
#   - returns: argparse.Namespace, [argname=argvalue, ...]

def parse_args():
    parser = argparse.ArgumentParser(
        description='Generate proxies and stubs from idl files',
    )
    parser.add_argument(
        'idlfiles',
        type=str,
        nargs='+',
        help='an idl file',
    )

    args = parser.parse_args()
    return args


# prints out program usage
def usage():
    print('usage: {} [-h] idlfiles [idlfiles...]'.format(sys.arv[0]))


##### IDL PROCESSING

# generate_shared
#   - generates c++ code shared between proxies and stubs
#
#   args:
#   - prefix [str]: the prefix of an idl file
#   - is_stub [bool]: true if code is for the stub, false if proxy
#
#   returns [str]: generated c++ code

def generate_shared(prefix, is_stub):
    headers = [
        '<cstdio>', '<cstring>', '<string>', '"c150debug.h"',
        '"rpc' + ('stub' if is_stub else 'proxy') + 'helper.h"',
        '"' + prefix + '.idl"'
    ]
    namespaces = ['std', 'C150NETWORK']

    return '\n'.join([
        generate_incls(headers, namespaces),
    ])


# generate_proxy
#   - generates proxy code for an idl file and saves it
#   - proxy file name: <prefix>.proxy.cpp
#
#   args:
#   - decls [dict]: idl file contents loaded as json into a python dict
#   - prefix [str]: the prefix of the idl file

def generate_proxy(decls, prefix):
    proxy_content = [
        generate_shared(prefix, False),
    ]

    with open(prefix + '.proxy.cpp', 'w+') as f:
        f.write('\n'.join(proxy_content))


# generate_stub
#   - generates stub code for an idl file and saves it
#   - stub file name: <prefix>.stub.cpp
#
#   args:
#   - decls [dict]: idl file contents loaded as json into a python dict
#   - prefix [str]: the prefix of the idl file

def generate_stub(decls, prefix):
    stub_content = [
        generate_shared(prefix, True),
    ]

    with open(prefix + '.stub.cpp', 'w+') as f:
        f.write('\n'.join(stub_content))


# generate
#   - generates and saves a proxy and stub for a given file
#   - if a file does not exist or cannot be opened, an error message is printed
#     and the function terminates
#
# args:
#   - fname [str]: fname, must be of the pattern *.idl
#
# returns: n/a

def generate(fname):
    if not isfile(fname):
        print("error: '{}' does not exist or could not be opened".format(fname))
        return
    else:
        prefix = get_file_prefix(fname) # save prefix for naming things
        if not prefix:
            print("error: '{}' must be named '<prefix>.idl'".format(fname))
            return

    # parse idl declarations into python dictionary
    decls = json.loads(subprocess.check_output(["./idl_to_json", fname])
        .decode('utf-8'))

    # generate files
    generate_proxy(decls, prefix)
    generate_stub(decls, prefix)


##### MAIN

def main():
    args = parse_args()
    for f in args.idlfiles:
        generate(f)


if __name__ == '__main__':
    main()
