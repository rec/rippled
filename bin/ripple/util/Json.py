from __future__ import absolute_import, division, print_function, unicode_literals

import json

def pretty_print(item):
    return json.dumps(item, sort_keys=True, indent=4, separators=(',', ': '))
