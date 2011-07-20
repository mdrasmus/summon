#!/usr/bin/env python-i

import time
import gc

import summon
from summon.core import *

'''
win = summon.Window()
for i in range(100):
    menu = summon.Menu()
    for i in range(10):
        menu.add_entry("a", lambda x: None)
        menu.add_submenu("b", summon.Menu())
    menu.delete()
'''

wins = []
for i in xrange(20):
    wins.append(summon.Window("%d" % i))

time.sleep(2)

for win in wins:
    win.close()


gc.collect()
