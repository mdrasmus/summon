#!/usr/bin/env python-i

import summon
from summon.core import *

nlines = 50000

win = summon.Window("lines")
win.set_visible(0, 0, nlines, 500)



#=============================================================================
start = summon.get_time()
for i in xrange(nlines):
    win.add_group(lines(i, 0,
                        i, 100))
print "add_group   ", summon.get_time() - start

win.clear_groups()


#=============================================================================
start = summon.get_time()
vis = group()
for i in xrange(nlines):
    vis.append(lines(i, 0, i, 100))
win.add_group(vis)
print "append      ", summon.get_time() - start

win.clear_groups()

#=============================================================================
start = summon.get_time()
vis = group()
l = []
for i in xrange(nlines):
    a = lines(i, 0, i, 100)
    l.append(a)
    vis.append(a)
win.add_group(vis)
print "append no GC", summon.get_time() - start

win.clear_groups()

#=============================================================================
start = summon.get_time()
vis = []
for i in xrange(nlines):
    vis.extend([i, 0, i, 100])
win.add_group(lines(*vis))
print "extend      ", summon.get_time() - start

win.clear_groups()


#=============================================================================
start = summon.get_time()
vis = []
n = float(nlines)
for i in xrange(nlines):
    vis.extend([color(i/n, 0, 1), i, 0, i, 100])
win.add_group(lines(*vis))
print "extend color", summon.get_time() - start

win.clear_groups()


#=============================================================================
def func():
    start = summon.get_time()
    vis = []
    for i in xrange(nlines):
        vis.extend([i, 0, i, 100])
    win.add_group(lines(*vis))
    print "1 thread    ", summon.get_time() - start

summon.add_timer(func, repeat=False)

