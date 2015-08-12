# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import os

from beast.build import BuildVariant, TagSet, Module
from beast.build.Module import Module
from beast.build.Function import compose, files, for_tags
from beast.build.module import Vcxproj

MODULES = (
)

CONFIG = {
    'build_dir': 'build/beast',

    'modules': [
        Module(
            files=files('beast/main.cpp'),

            target=for_tags(
                'unity', 'default_toolchain', 'release',
                BuildVariant.add_to_default_target('beast'),
            ),

            after=Vcxproj.after(
                os.path.join('Builds', 'VisualStudio2013', 'Beast'),
                source=[],
                VSPROJECT_ROOT_DIRS=['.'],
            ),
        ),
    ],

    'targets': [
        TagSet.Target('install', TagSet.CPP_GROUPS, program_name='beast'),
        TagSet.Target('vcxproj'),
    ],

    'environment': {
        'toolpath': [os.path.join('python', 'site_tools')],
        'tools': ['default', 'Protoc', 'VSProject'],
        'ENV': os.environ,
        'TARGET_ARCH': 'x86_64',
    },

}
