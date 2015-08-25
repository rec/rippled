# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

from beast.build import Function

"""
Modules are called in four phases:  before, files, target, after.

The "before" phase is before any files are added, and the module is called
once with a BuildState.

The "files" phase is when the Module gets to add files.  It's called
once for each variant.

The "target" phase happens after all the files have been added, and once the
target has been given a value.  It's also called for each variant.

Finally, the "after" phase happens after all the variants have been processed.
This is where you can add things like PhonyTargets.
"""

class Module(object):
    def __init__(self, before=None, files=None, target=None, after=None):
        nothing = lambda build: None
        self.before = before or nothing
        self.target = target or nothing
        self.after = after or nothing

        # Make it a bit more user-friendly.
        files = files or nothing
        if isinstance(files, (unicode, str)):
            files = files
        if not callable(files):
            files = Function.files(*files)
        self.files = files

    def run(self, phase, build):
        getattr(self, phase)(build)


class Composer(object):
    """Combine a series of modules through composition."""
    def __init__(self, *modules):
        self.modules = modules

    def run(self, phase, build):
        for module in self.modules:
            module.run(phase, build)

compose = Composer
