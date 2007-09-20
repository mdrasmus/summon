#!/usr/bin/env python-i

from summon.core import *
import summon
import time


def on_move(x, y):
    print x, y

win = summon.Window("123")
win.set_position(100,100)
win.on_move = on_move

for i in xrange(10):
    print i, win.get_position()
    time.sleep(.2)



win.set_position(200, 200)
print win.get_position()
