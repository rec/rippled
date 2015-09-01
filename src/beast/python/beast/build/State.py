# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import collections, copy, os

from . CheckForFastBuild import check_for_fast_build
from . import Variant, Module, Target, Toolchain
from .. System import SYSTEM


class State(object):
    """
    Represents the state of a scons build environment.
    """
    def __init__(self, sconstruct_globals, environment, variant_tree, build_dir,
                 environ=None):
        class Sconstruct(object):
            """Turn the SConstruct globals() back into a namespace,
            so you can write things like self.sconstruct.COMMAND_LINE_TARGETS.
            """
            def __getattr__(self, key):
                return sconstruct_globals[key]

        self.sconstruct = Sconstruct()
        self.env = self.sconstruct.Environment(**environment)
        self.variant_tree = variant_tree
        self.build_dir = build_dir
        self.environ = environ or sconstruct_globals.get('ENV', os.environ)
        self.aliases = collections.defaultdict(list)
        self.msvc_configs = []
        self.tags = [SYSTEM.platform.lower()]
        if SYSTEM.linux:
            self.tags.append('linux')

        if SYSTEM.osx:
            self.tags.append('osx')

        self.subst = getattr(self.env, 'subst', lambda x: x)
        check_for_fast_build(self)

    def get_environment_variable(self, key, default=None):
        return self.subst(self.environ.get(key, default))

    def run_build(self, modules, targets):
        targets = Target.Targets(*targets)
        module = Module.compose(*modules)
        module.before(self)

        # Configure the toolchains, variants, default toolchain, and default
        # target.
        toolchains = Toolchain.detect(self)
        if not toolchains:
            raise ValueError('No toolchains detected!')

        target_line = list(self.sconstruct.COMMAND_LINE_TARGETS)

        ttt = targets.targets_to_tags(target_line)
        print('!!!', ttt)
        for target, tags_list in targets.targets_to_tags(target_line).items():
            pname = target.result_name
            if pname:
                for tags in tags_list:
                    Variant.add_variant(self, tags, toolchains, pname, module)

        for key, value in self.aliases.iteritems():
            self.env.Alias(key, value)

        module.after(self)


DEFAULTS = {
    'build_dir': 'build',

    'environment': {
        'toolpath': [os.path.join('src', 'beast', 'python', 'site_tools')],
        'tools': ['default', 'Protoc', 'VSProject'],
        'ENV': os.environ,
        'TARGET_ARCH': 'x86_64',
    },

    'variant_tree': {'src': 'src'},
}


def _run_build(sconstruct_globals, environment, variant_tree, build_dir,
            modules, targets):
    state = State(sconstruct_globals, environment, variant_tree, build_dir)
    return state.run_build(modules, targets)


def run_build(**kwds):
    _run_build(**dict(DEFAULTS, **kwds))
