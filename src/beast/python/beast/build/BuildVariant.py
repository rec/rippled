# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import collections, copy, os

from beast.build.Util import list_sources, variant_file
from beast.build.CcWarning import update_warning_kwds

from beast.System import SYSTEM

ALL_TARGET = 'all'

class BuildVariant(object):
    def __init__(self, state, tags, toolchains, program):
        self.state = state
        self.tags = list(tags)
        self.toolchains = toolchains
        self.program = program

        self.variant_name = '.'.join(self.tags).replace('.unity', '')
        self.variant_directory = os.path.join(
            self.state.build_dir, self.variant_name)
        self.toolchain, self.variant = self.tags[:2]
        self.default_toolchain = toolchains.keys()[0]
        if self.toolchain == self.default_toolchain:
            self.tags.append('default_toolchain')

        self.tags.append(SYSTEM.platform.lower())
        if SYSTEM.linux:
            self.tags.append('linux')

        if SYSTEM.osx:
            self.tags.append('osx')

        self.env = state.env.Clone()
        self.env.Replace(**toolchains.get(self.toolchain, {}))
        self.objects = []
        self.files_seen = set()

    def variant_directory_tree(self):
        vdir = self.variant_directory
        items = self.state.variant_tree.items()
        return {os.path.join(vdir, k): v for (k, v) in items}

    def run(self, module):
        # Add all the files.
        module.run('files', self)

        # Now produce all the variant trees.
        for dest, source in self.variant_directory_tree().items():
            self.env.VariantDir(dest, source, duplicate=0)

        # Finally, make the program target.
        self.target = self.env.Program(
            target=os.path.join(self.variant_directory, self.program),
            source=self.objects)

        # Now we run the "target" phase.
        module.run('target', self)
        if self.toolchain in self.toolchains:
            self.state.aliases[self.variant].extend(self.target)
            self.env.Alias(self.variant_name, self.target)

    def add_source_files(self, *filenames, **kwds):
        update_warning_kwds(self.tags, kwds)
        variants = self.variant_directory_tree()
        for filename in filenames:
            vfile = variant_file(filename, variants)
            if vfile in self.files_seen:
                print('WARNING: target %s was seen twice.' % vfile)
                continue
            env = self.env
            if kwds:
                env = env.Clone()
                env.Prepend(**kwds)
            self.objects.append(env.Object(vfile))

    def add_source_by_directory(self, *directories, **kwds):
        for d in directories:
            self.add_source_files(*list_sources(d, '.cpp'), **kwds)


def add_to_all_target(all_target=ALL_TARGET):
    def f(variant):
        if variant.toolchain in variant.toolchains:
            variant.state.aliases[all_target].extend(variant.target)
            variant.state.aliases[variant.toolchain].extend(variant.target)

    return f

def add_to_default_target(default_target, all_target=ALL_TARGET):
    def f(variant):
        install_target = variant.env.Install(
            variant.state.build_dir, source=variant.target)
        variant.env.Alias(default_target, install_target)
        variant.env.Default(install_target)
        variant.state.aliases[all_target].extend(install_target)

    return f
