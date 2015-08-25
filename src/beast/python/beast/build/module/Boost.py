# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import os
from beast.build import Function, Module

def boost(variant, link_libraries):
    link_libraries = [i if i.startswith('boost_') else 'boost_' + i
                      for i in link_libraries]
    root = variant.state.environ.get('BOOST_ROOT')
    if root:
        root = os.path.normpath(root)
        variant.env['BOOST_ROOT'] = root

        # We prefer static libraries for boost
        static_libs = ['%s/stage/lib/lib%s.a' % (root, l)
                       for l in link_libraries]
        if all(os.path.exists(f) for f in static_libs):
            link_libraries = [variant.state.sconstruct.File(f)
                              for f in static_libs]
            variant.env.Append(CPPPATH=[root],
                               LIBPATH=[os.path.join(root, 'stage', 'lib')])

    variant.env.Append(LIBS=link_libraries + ['dl'])


def module(link_libraries=None):
    """Load Boost with the given precompiled link_libraries if any."""
    return Module.Module(
        files=lambda variant: boost(variant, link_libraries or []),
        before=Function.for_tags(
            'clang',
            Function.Env.Append(
                CPPDEFINES=['BOOST_ASIO_HAS_STD_ARRAY'],
            ),
        ),
    )