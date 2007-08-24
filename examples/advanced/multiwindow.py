#!/usr/bin/env python-i
# NOTE: These features are still experimental
#


import sys
from summon.core import *
from summon.multiwindow import *
import summon


rows = ["row%d" % x for x in range(1,41)]
cols = ["col%d" % x for x in range(1,41)]



win1 = summon.Window("window 1")
win2 = summon.Window("window 2")
win3 = summon.Window("window 3")
win4 = summon.Window("window 4")
win5 = summon.Window("window 5")

for w in [win1, win2, win3, win4, win5]:
    w.set_bgcolor(1, 1, 1)


vis = [color(0,0,0)]
for i, row in enumerate(rows):
    vis.append(text_clip(row, -100, -i, 0, -i-1, 3, 20, "right", "middle"))
win4.add_group(group(color(0,0,0), lines(0,0, 0, -len(rows)), *vis))
win5.add_group(group(color(0,0,0), lines(0,0, 0, -len(rows)), *vis))

win1.add_group(group(color(0,0,0), lines(0, -len(rows), len(cols), 0)))
win2.add_group(group(color(0,0,0), lines(0, -len(rows), len(cols), 0)))
win3.add_group(group(color(0,0,0), lines(0, -len(rows), len(cols), 0)))
#win4.add_group(group(lines(0, 0, 100,100)))
#win5.add_group(group(lines(0, 0, 100,100)))

win4.set_visible(-10, 0, 0, -len(rows))
win5.set_visible(-10, 0, 0, -len(rows))
win2.set_visible(0, -len(rows), len(cols), 0)

win1.set_size(400, 100)
win2.set_size(400, 400)
win3.set_size(400, 100)
win4.set_size(100, 400)
win5.set_size(100, 400)

       
e = WindowEnsemble([win1, win2, win3], 
                   stackx=True, samew=True, tiex=True)#, pinx=True)
e = WindowEnsemble([win4, win2, win5], 
                   stacky=True, sameh=True, tiey=True)#, piny=True)

summon.begin_updating()
