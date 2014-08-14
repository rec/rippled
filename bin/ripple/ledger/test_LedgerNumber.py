from __future__ import absolute_import, division, print_function, unicode_literals

from ripple.ledger import LedgerNumber

from unittest import TestCase

class test_LedgerNumber(TestCase):
    def test_check(self):
        self.assertEquals(LedgerNumber.check(None), None)
        self.assertEquals(LedgerNumber.check(LedgerNumber.FIRST_EVER),
                          LedgerNumber.FIRST_EVER)
        self.assertEquals(LedgerNumber.check(str(LedgerNumber.FIRST_EVER)),
                          LedgerNumber.FIRST_EVER)
        self.assertEquals(LedgerNumber.check('1000000'), 1000000)
        for n in LedgerNumber.LEDGERS:
            self.assertEquals(LedgerNumber.check(n), n)
        self.assertRaises(ValueError, LedgerNumber.check, 'last')
        self.assertRaises(ValueError, LedgerNumber.check, 'xyzzy')

    # Set up variables to look like a reader!
    closed = 2000000
    current = 2000001
    first = 1000000
    validated = 1999999

    def test_to_number(self):
        self.assertEquals(LedgerNumber.to_number(self, 'closed'),
                          self.closed)
        self.assertEquals(LedgerNumber.to_number(self, 'current'),
                          self.current)
        self.assertEquals(LedgerNumber.to_number(self, 'first'),
                          self.first)
        self.assertEquals(LedgerNumber.to_number(self, 'validated'),
                          self.validated)

        self.assertEquals(LedgerNumber.to_number(self, 'firstever'),
                          LedgerNumber.FIRST_EVER)

        self.assertEquals(LedgerNumber.to_number(self, 3000000), 3000000)
        self.assertRaises(AttributeError,
                          LedgerNumber.to_number, self, 'wombat')
