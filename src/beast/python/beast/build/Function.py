# This file is part of Beast: http://github.com/vinniefalco/Beast
#
# This file copyright (c) 2015, Tom Ritchford <tom@swirly.com>
# under the Boost software license http://www.boost.org/LICENSE_1_0.txt

from __future__ import (
    absolute_import, division, print_function, unicode_literals)

import glob, os
from functools import wraps

"""
This module is concerned with functions of a single variable, which
will be either a BuildState or a BuildVariant.
"""

def compose(*functions):
    def wrapped(x):
        for f in functions:
            f(x)
    return wrapped


def _tagger(items, matcher):
    tags, functions = set(), []

    for i in items:
        (functions.append if callable(i) else tags.add)(i)

    def wrapped(build):
        t = set(getattr(build, 'tags', ()))
        if matcher(tags, t):
            for f in functions:
                f(build)

    return wrapped

def for_tags(*items):
    return _tagger(items, lambda tags, t: not (tags - t))

def not_tags(*items):
    return _tagger(items, lambda tags, t: not (tags & t))


class EnvClass(object):
    """EnvClass has attributes that are _functions_ that call methods on a
    member that's an Env class.

    Example: the next two statements have the same effect:
       EnvClass().Append(CXXFLAGS='-g')(env)
       env.Append(CXXFLAGS='-g')

    """
    def __getattr__(self, name):
        def f(*args, **kwds):
            # build can be anything with a .env member - i.e. either a
            # BuildVariant or a BuildState.
            return lambda build: getattr(build.env, name)(*args, **kwds)
        return f

# Env is a unique instance of EnvClass.  It's really useful for creating
# functions!
Env = EnvClass()


def pkg_config(name):
    """Return a function that runs pkg-config for the tool"""
    return Env.ParseConfig('pkg-config --static --cflags --libs ' + name)


def _all_globs(globs):
    for g in globs:
        files = glob.glob(g)
        if files:
            for f in files:
                yield f
        else:
            print('ERROR: Glob %s didn\'t match any files.' % g)


def files(*globs, **kwds):
    """Return a function that adds source files.
    """
    def run(variant):
        variant.add_source_files(*_all_globs(globs), **kwds)
    return run


def directories(*globs, **kwds):
    """Return a function that adds source files recursively contained in
    directories.
    """
    def run(variant):
        variant.add_source_by_directory(*_all_globs(globs), **kwds)

    return run
