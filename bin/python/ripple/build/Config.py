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
