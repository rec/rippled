from __future__ import absolute_import, division, print_function, unicode_literals

FIRST_EVER = 32570

LEDGERS = {
    'closed': 'the most recently closed ledger',
    'complete': 'the last complete ledger',
    'current': 'the current ledger;',
    'first': 'the first ledger on this server',
    'firstever': 'the first-ever ledger',
    'validated': 'the most recently validated ledger',
    }

HELP = """
Ledgers are either represented by a number, or one of the special ledgers;
""" + ',\n'.join('%s, %s' % (k, v) for k, v in sorted(LEDGERS.items())
)

def to_number(reader, name):
    if isinstance(name, int):
        return name
    if name == 'firstever':
        return FIRST_EVER
    return getattr(reader, name)

def check(name):
    """Make sure that a string name is a legal ledger number or None."""
    if name is None or name in LEDGERS:
        return name

    try:
        name = int(name)
    except:
        raise ValueError("Don't understand ledger name " + name)

    if name < FIRST_EVER:
        raise ValueError("Ledger name %d can't be less than %d" %
                         (name, FIRST_EVER))
    return name
