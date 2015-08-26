# rippled SConstruct
#

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

"""

    Target          Builds
    ----------------------------------------------------------------------------

    <none>          Same as 'install'
    install         Default target and copies it to build/rippled (default)

    all             All available variants
    debug           All available debug variants
    release         All available release variants
    profile         All available profile variants

    clang           All clang variants
    clang.debug     clang debug variant
    clang.release   clang release variant
    clang.profile   clang profile variant

    gcc             All gcc variants
    gcc.debug       gcc debug variant
    gcc.release     gcc release variant
    gcc.profile     gcc profile variant

    msvc            All msvc variants
    msvc.debug      MSVC debug variant
    msvc.release    MSVC release variant

    vcxproj         Generate Visual Studio 2013 project file

    count           Show line count metrics

    Any individual target can also have ".nounity" appended for a classic,
    non unity build. Example:

        scons gcc.debug.nounity

If the clang toolchain is detected, then the default target will use it, else
the gcc toolchain will be used. On Windows environments, the MSVC toolchain is
also detected.

The following environment variables modify the build environment:
    CLANG_CC
    CLANG_CXX
    CLANG_LINK
      If set, a clang toolchain will be used. These must all be set together.

    GNU_CC
    GNU_CXX
    GNU_LINK
      If set, a gcc toolchain will be used (unless a clang toolchain is
      detected first). These must all be set together.

    CXX
      If set, used to detect a toolchain.

    BOOST_ROOT
      Path to the boost directory.
    OPENSSL_ROOT
      Path to the openssl directory.

"""

#
"""

TODO

- Fix git-describe support
- Fix printing exemplar command lines
- Fix toolchain detection


"""
#-------------------------------------------------------------------------------

import os, sys

sys.path.append(os.path.join('src', 'beast', 'python'))
sys.path.append(os.path.join('bin', 'python'))

from beast.build import Build
from ripple.build.Config import CONFIG

Build.run(sconstruct_globals=globals(), **CONFIG)
