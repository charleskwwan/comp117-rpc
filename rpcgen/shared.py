# shared.py
#
# Defines functions to generate c++ code that is shared between proxies and
# stubs
#
# by: Justin Jo and Charles Wan


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
    return '{}\n\n{}'.format(
        '\n'.join(['#include ' + h for h in headers]), # includes
        '\n'.join(['using namespace ' + n + ';' for n in namespaces]), # nspaces
    )
