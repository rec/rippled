from __future__ import absolute_import, division, print_function, unicode_literals

import os

def normalize(f):
    f = os.path.join(*f.split('/'))
    return os.path.expanduser(os.path.abspath(f))
