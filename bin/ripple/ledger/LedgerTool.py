from __future__ import absolute_import, division, print_function, unicode_literals

import json
import os

from ripple.ledger import FileReader, RippledReader, ServerReader
from ripple.ledger.Args import ARGS
from ripple.util.FileCache import file_cache

class LedgerTool(object):
    def __init__(self):
        if ARGS.file:
            reader = FileReader.FileReader()
        elif ARGS.rippled:
            reader = RippledReader.RippledReader()
        else:
            reader = ServerReader.ServerReader()

        self.reader = reader

        self.closed = reader.name_to_ledger_index('closed')
        self.current = reader.name_to_ledger_index('current')
        self.validated = reader.name_to_ledger_index('validated')

        def make_cache(is_full):
            name = 'full' if is_full else 'summary'
            filepath = os.path.join(ARGS.cache, 'ledger', name)
            return file_cache(
                filepath, lambda n: reader.get_ledger(n, is_full), json.loads)
        self.caches = [make_cache(False), make_cache(True)]

    def info(self):
        return self.reader.info

    def get_ledger(self, number, is_full=False):
        return self.caches[is_full](number, number <= self.validated)
