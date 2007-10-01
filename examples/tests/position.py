#!/usr/bin/env python-i

from summon.core import *
import summon
import time


def on_move(x, y):
    print x, y

win = summon.Window("123")
win.set_position(100,100)


if 0:
    for i in xrange(10):
        print i, win.get_position()
        time.sleep(.2)



win.set_position(200, 200)
print win.get_position()

win2 = summon.Window(" ")
win2.set_size(100, 100)
#print win2.get_position()
print win2.get_size()
