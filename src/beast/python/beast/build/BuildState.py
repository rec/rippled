# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import collections, copy, os

from beast.build.Util import DictToAttr
from beast.build.CheckForFastBuild import check_for_fast_build
from beast.build import BuildVariant, Module, TagSet, Toolchain

class BuildState(object):
    """
    Represents the state of a scons build environment.
    """
    def __init__(self, sconstruct_globals, environment, variant_tree, build_dir,
                 environ=None):
        self.sconstruct = DictToAttr(**sconstruct_globals)
        self.env = self.sconstruct.Environment(**environment)
        self.variant_tree = variant_tree
        self.build_dir = build_dir
        self.environ = environ or sconstruct_globals.get('ENV', os.environ)
        self.aliases = collections.defaultdict(list)
        self.msvc_configs = []

        self.subst = getattr(self.env, 'subst', lambda x: x)
        check_for_fast_build(self)

    def get_environment_variable(self, key, default=None):
        return self.subst(self.environ.get(key, default))

    def run(self, modules, targets):
        targets = TagSet.Targets(*targets)
        module = Module.compose(*modules)
        module.run('before', self)

        # Configure the toolchains, variants, default toolchain, and default
        # target.
        toolchains = Toolchain.detect(self)
        if not toolchains:
            raise ValueError('No toolchains detected!')

        target_line = list(self.sconstruct.COMMAND_LINE_TARGETS) or [
            targets.default_argument]

        for target, tags_list in targets.targets_to_tags(target_line).items():
            if target.program_name:
                for tags in tags_list:
                    variant = BuildVariant.BuildVariant(
                        self, tags, toolchains, target.program_name)
                    variant.run(module)

        for key, value in self.aliases.iteritems():
            self.env.Alias(key, value)

        module.run('after', self)


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


def _run(sconstruct_globals, environment, variant_tree, build_dir,
            modules, targets):
    state = BuildState(sconstruct_globals, environment, variant_tree, build_dir)
    return state.run(modules, targets)


def run(**kwds):
    _run(**dict(DEFAULTS, **kwds))
