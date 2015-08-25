# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import os
from beast.build import Function, Module

def module(architecture='x64'):
    def target(variant):
        suffix = '.classic' if ('nounity' in variant.tags) else ''
        config = variant.env.VSProjectConfig(
            variant.variant + suffix, architecture, variant.target, variant.env)
        variant.state.msvc_configs.append(config)

    return Module.Module(
        target=Function.for_tags('msvc', target),
    )

MODULE = module()