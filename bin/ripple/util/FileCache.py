from __future__ import absolute_import, division, print_function, unicode_literals

import os

_NONE = object()

def file_cache(filename_prefix, creator, processor):
    """A two-level cache, which stores expensive results in memory and on disk.
    """
    cached_data = {}

    def get_file_data(name):
        filename = os.path.join(filename_prefix, name)
        if os.exists(filename):
            return open(filename).reader()

        result = creator(name)
        open(filename, 'w').write(result)
        return result

    def get_data(name, use_file_cache=True):
        result = cached_data.get(name, _NONE)
        if result is _NONE:
            maker = get_file_data if use_file_cache else creator
            result = processor(maker(name))
            cached_data[name] = result
        return result

    return get_data
