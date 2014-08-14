from __future__ import absolute_import, division, print_function, unicode_literals

import argparse
import importlib
import os

from ripple.ledger import LedgerNumber
from ripple.util import File

NAME = 'LedgerTool'
VERSION = '0.1'

_parser = argparse.ArgumentParser(
    prog=NAME,
    description='Retrieve and process Ripple ledgers.',
    epilog=LedgerNumber.HELP,
    )

# Positional arguments.
_parser.add_argument(
    'command',
    nargs='*',
    default='print',
    help='Command to execute.'
)

# Flag arguments.
_parser.add_argument(
    '--begin', '-b',
    default='current',
    help='The first ledger to request.',
    )

_parser.add_argument(
    '--cache',
    default='~/.rippled/.local',
    help='The cache directory.',
    )

_parser.add_argument(
    '--condition', '-c',
    help='The name of a condition function used to match ledgers.',
    )

_parser.add_argument(
    '--config',
    help='The rippled configuration file name.',
    )

_parser.add_argument(
    '--display', '-d',
    choices=['all', 'first', 'last'],
    default='first',
    help='Select which ledgers to display.',
    )

_parser.add_argument(
    '--end', '-e',
    help='The last ledger to request. Defaults to --begin.',
    )

_parser.add_argument(
    '--file', '-f',
    help='The filename of a text file containing JSON to parse.'
    )

_parser.add_argument(
    '--indent', '-i',
    type=int,
    default=4,
    help='How many spaces to indent when display in JSON.',
    )

_parser.add_argument(
    '--linear', '-l',
    action='store_true',
    help='If true, searches are linear - by default binary search is used.',
    )

_parser.add_argument(
    '--rippled', '-r',
    help='The filename of a rippled binary for retrieving ledgers.',
    )

_parser.add_argument(
    '--server', '-s',
    help='IP address of a rippled JSON server.',
    )

_parser.add_argument(
    '--version', '-v',
    action='version',
    version='%(prog)s ' + VERSION,
    help='Print the current version of %(prog)s',
    )

_parser.add_argument(
    '--window', '-w',
    type=int,
    default=0,
    help='How many ledgers to display around the matching ledger.',
    )

_parser.add_argument(
    '--yes', '-y',
    action='store_true',
    help='If true, don\'t ask for confirmation on large commands.',
)

# Read the arguments from the command line.
ARGS = _parser.parse_args()

# Make sure that begin and end are possible ledgers.
ARGS.begin = LedgerNumber.check(ARGS.begin)
ARGS.end = LedgerNumber.check(ARGS.end)

ARGS.cache = File.normalize(ARGS.cache)

if ARGS.condition:
    # Conditions are dynamically loaoded Python.
    p, m = name.rsplit('.', 1)
    try:
        ARGS.condition = getattr(importlib.import_module(p), m)
    except:
        raise ValueError('Can\'t understand --condition=' + ARGS.condition)

if ARGS.window < 0:
    raise ValueError('Window cannot be negative: --window=%d' %
                     ARGS.window)

_loaders = bool(ARGS.server) + bool(ARGS.file) + bool(ARGS.rippled)

if not _loaders:
    ARGS.rippled = 'rippled'

elif _loaders > 1:
    raise ValueError('At most one of --file, --rippled and --server'
                     ' must be specified')
