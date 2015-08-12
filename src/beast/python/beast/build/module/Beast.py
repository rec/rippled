# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import os

from beast.build.Module import Module
from beast.build.Function  import Env, compose, files, for_tags

MODULE = Module(
    before=compose(
        Env.Append(
            CPPPATH=['src', os.path.join('src', 'beast')],
        ),

        for_tags(
            'darwin',
            Env.Append(
                CPPDEFINES=[{'BEAST_COMPILE_OBJECTIVE_CPP': 1}],
            ),
        ),
    ),

    files=compose(
        files(
            'src/beast/beast/unity/hash_unity.cpp',
            'src/beast/beast/unity/beast.cpp',
        ),

        for_tags(
            'osx',
            files('src/beast/beast/unity/beastobjc.mm'),
        ),
    ),
)
