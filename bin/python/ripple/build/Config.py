# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import os

from beast.build import BuildState, TagSet
from ripple.build.module import MODULES

CONFIG = {
    'modules': MODULES,

    'targets': [
        TagSet.Target('install', TagSet.CPP_GROUPS, program_name='rippled'),
        TagSet.Target('vcxproj'),
        TagSet.Target('count_tests'),
    ],
}
