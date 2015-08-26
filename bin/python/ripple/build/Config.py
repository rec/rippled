from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import os

from beast.build.Target import Target
from .module.Modules import MODULES

CONFIG = {
    'modules': MODULES,

    'targets': [
        Target('install', Target.CPP_GROUPS, program_name='rippled'),
        Target('vcxproj'),
        Target('count_tests'),
    ],
}
