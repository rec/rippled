# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)


ALL_TARGET = 'all'

def add_to_all(all_target=ALL_TARGET):
    def f(variant):
        if variant.toolchain in variant.toolchains:
            variant.state.aliases[all_target].extend(variant.target)
            variant.state.aliases[variant.toolchain].extend(variant.target)

    return f

def add_to_default(default_target, all_target=ALL_TARGET):
    def f(variant):
        install_target = variant.env.Install(
            variant.state.build_dir, source=variant.target)
        variant.env.Alias(default_target, install_target)
        variant.env.Default(install_target)
        variant.state.aliases[all_target].extend(install_target)

    return f
