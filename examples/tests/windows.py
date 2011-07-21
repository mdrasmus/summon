#!/usr/bin/env python-i

import time
import gc

import summon
from summon.core import *




wins = []
for i in xrange(5):
    wins.append(summon.Window("%d" % i))

time.sleep(10)

for win in wins:
    win.close()

time.sleep(2)

