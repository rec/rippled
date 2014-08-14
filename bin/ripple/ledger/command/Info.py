from __future__ import absolute_import, division, print_function, unicode_literals

from ripple.util.Json import pretty_print

import json

SAFE = True

HELP = 'info - return server_info'

def info(ledger_tool):
    print(pretty_print(ledger_tool.info()))
