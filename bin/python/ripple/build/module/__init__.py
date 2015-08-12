# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import os

from beast.build import BuildVariant
from beast.build.Module import Module
from beast.build.module import (
    Beast, Boost, CountTests, GitTag, Jemalloc, MSVC, OpenSSL, Proto, Vcxproj)
from ripple.build import Unity
from ripple.build.module import Default

from beast.build.Function import Env, compose, files, for_tags, not_tags


BOOST = Boost.module(
    link_libraries=[
        'coroutine',
        'context',
        'date_time',
        'filesystem',
        'program_options',
        'regex',
        'system',
        'thread',
    ],
)

BEAST = Beast.MODULE

COUNT_TESTS = CountTests.module(os.path.join('src', 'ripple'))

ED25519 = Module(
    files=files(
        'src/ripple/unity/ed25519.c',
        CPPPATH=['src/ed25519-donna'],
    ),
)

GIT_TAG = Module(
    files=GitTag.git_tag('src/ripple/unity/git_id.cpp'))

LEGACY = Module(
    files=compose(
        Unity.files(
            unity_files=[
                'app_ledger.cpp',
                'app_main.cpp',
                'app_misc.cpp',
                'app_paths.cpp',
                'app_tests.cpp',
                'app_tx.cpp',
                'core.cpp',
            ],
            nounity_directories=['app', 'legacy'],
        ),

        files(
            'src/ripple/unity/lz4.c',
            'src/ripple/unity/resource.cpp',
            'src/ripple/unity/server.cpp',
        ),
    ),
)

NODE_STORE = Module(
    files=Unity.files(
        modules=['nodestore'],
        CPPPATH=[
            'src/rocksdb2/include',
            'src/snappy/snappy',
            'src/snappy/config',
        ],
    ),
)

PROTO = Module(
    before=Proto.before('src/ripple/proto/ripple.proto'),

    files=compose(
        Proto.files,
        files(
            'src/ripple/unity/protobuf.cpp',
            'src/ripple/unity/ripple.proto.cpp',
        ),
    ),
)

UNITY = Module(
    files=Unity.files(
        modules=[
            'basics',
            'crypto',
            'json',
            'ledger',
            'net',
            'overlay',
            'peerfinder',
            'protocol',
            'rpc',
            'shamap',
            'test',
            'unl',
        ],
    ),
)

ROCKS_DB = Module(
    files = files(
        'src/ripple/unity/rocksdb.cpp',
        CPPPATH=[
            'src/rocksdb2',
            'src/rocksdb2/include',
            'src/snappy/snappy',
            'src/snappy/config'],
        disable_warnings='maybe-uninitialized',
    ),
)

SECP256K1 = Module(
    files=files(
        'src/ripple/unity/secp256k1.cpp',
        CPPPATH=['src/secp256k1'],
        disable_warnings='unused-function',
    ),
)

SOCI = Module(
    before=compose(
        Env.Append(
            CPPPATH=[
                os.path.join('src','soci','src'),
                os.path.join('src','soci','include'),
            ],
        ),
    ),

    files=compose(
        files(
            'src/ripple/unity/soci.cpp',
            CPPPATH=[
                'src/soci/src/core',
                'src/soci/include/private',
                'src/sqlite',
            ],
            disable_warnings='deprecated-declarations',
        ),

        Unity.files(
            unity_files=['soci_ripple.cpp'],
            nounity_directories=['core'],
            CPPPATH=['src/soci/src/core', 'src/sqlite'],
        ),
    )
)

SNAPPY = Module(
    files=files(
        'src/ripple/unity/snappy.cpp',
        CPPPATH=['src/snappy/snappy', 'src/snappy/config'],
        disable_warnings='unused-function',
    ),
)

SQLITE = Module(
    files=files(
            'src/ripple/unity/sqlite.c',
            disable_warnings='array-bounds',
        ),
)


TARGETS = Module(
    target=compose(
        for_tags(
            'unity', 'default_toolchain', 'release',
            BuildVariant.add_to_default_target('install'),
        ),

        for_tags(
            'unity',
            BuildVariant.add_to_all_target(),
        ),
    ),
)

VCXPROJ = Vcxproj.module(
    os.path.join('Builds', 'VisualStudio2013', 'RippleD'),
    source=[],
    VSPROJECT_ROOT_DIRS=['src/beast', 'src', '.'],
)

WEBSOCKETS = Module(
    files=compose(
        files('src/ripple/unity/websocket02.cpp'),
        files('src/ripple/unity/websocket04.cpp', CPPPATH='src/websocketpp'),
    ),
)

MODULES = (
    Default.MODULE,
    BOOST,
    BEAST,
    COUNT_TESTS,
    ED25519,
    GIT_TAG,
    Jemalloc.MODULE,
    LEGACY,
    MSVC.MODULE,
    NODE_STORE,
    OpenSSL.MODULE,
    PROTO,
    ROCKS_DB,
    SECP256K1,
    SNAPPY,
    SOCI,
    SQLITE,
    TARGETS,
    UNITY,
    VCXPROJ,
    WEBSOCKETS,
)
