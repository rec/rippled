#!/usr/bin/env python

from __future__ import absolute_import, division, print_function, unicode_literals

import pprint
import sys

from ripple.ledger import LedgerTool
from ripple.ledger.Args import ARGS
from ripple.util.CommandList import CommandList

from ripple.ledger.command import Info

_COMMANDS = CommandList(Info)

if __name__ == '__main__':
    tool = LedgerTool.LedgerTool()
    args = list(ARGS.command)
    command = args.pop(0)
    _COMMANDS.run_safe(command, tool, *args)
