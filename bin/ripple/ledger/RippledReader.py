from __future__ import absolute_import, division, print_function, unicode_literals

import json
import os
import subprocess

from ripple.ledger.Args import ARGS
from ripple.util import File

_ERROR_CODE_REASON = {
    62: 'No rippled server is running.',
}

_DEFAULT_ERROR_ = "Couldn't connect to server."

def complete(desc):
    result = set()
    if desc and desc != 'empty':
        for part in desc.split(','):
            range = part.split('-')
            if len(range) == 1:
                result.add(int(part))
            elif len(range) == 2:
                result.update(xrange(int(range[0]), int(range[1]) + 1))
    return result

class RippledReader(object):
    def __init__(self):
        fname = File.normalize(ARGS.rippled)
        if not os.path.exists(fname):
            raise Exception('No rippled found at %s.' % fname)
        self.cmd = [fname]
        if ARGS.config:
            self.cmd.extend(['--conf', _normalize(ARGS.config)])
        self.info = self._command('server_info')['info']
        self.complete = complete(self.info.get('complete_ledgers'))

    def name_to_ledger_index(self, ledger_name, is_full=False):
        try:
            info = self.get_ledger(ledger_name, is_full)
        except:
            return 0
        return info['ledger']['ledger_index']

    def get_ledger(self, ledger_name, is_full=False):
        if is_full:
            return self._command('ledger', ledger_name, 'full')
        else:
            return self._command('ledger', ledger_name)

    def _command(self, *cmds):
        cmd = self.cmd + list(cmds)
        try:
            return json.loads(subprocess.check_output(
                cmd, stderr=subprocess.PIPE))['result']
        except subprocess.CalledProcessError as e:
            raise Exception(_ERROR_CODE_REASON.get(
                e.returncode, _DEFAULT_ERROR_))
