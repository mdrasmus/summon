#!/usr/bin/env python-i

from summon.core import *
import summon
import time

win = summon.Window("123")
win.set_position(100,100)


for i in xrange(10):
    print i, win.get_position()
    time.sleep(.2)
