#!/usr/bin/env python-i
# NOTE: These features are still experimental
#


import sys
from summon.core import *
from summon.multiwindow import *
import summon

nrows = 40
ncols = 40



def make_labels(rows, rot=False):
    vis = [color(0,0,0), 
           lines(0,0, 0, -len(rows),
                 0,0, -10, 0,
                 0, -len(rows), -6, -len(rows))]
    for i, row in enumerate(rows):
        if rot:
            vis.append(text_clip(row, -100, -i, 0, -i-1, 3, 20, 
                                 "right", "middle", "vertical"))
        else:
            vis.append(text_clip(row, -100, -i, 0, -i-1, 3, 20, 
                                 "right", "middle"))
    
    if rot:
        return rotate(90, *vis)
    else:
        return group(*vis)

def startup():
    pos = (500, 500)
    win1 = summon.Window("window 1", size=(400, 100), position=(300, 200))
    win2 = summon.Window("window 2", size=(400, 400), position=(300, 300))
    win3 = summon.Window("window 3", size=(400, 100), position=(300, 700))
    win4 = summon.Window("window 4", size=(100, 400), position=(200, 300))
    win5 = summon.Window("window 5", size=(100, 400), position=(700, 300))

    for w in [win1, win2, win3, win4, win5]:
        w.set_bgcolor(1, 1, 1)

    # create labels
    rows = ["row%d" % (x+1) for x in range(nrows)]
    cols = ["col%d" % (x+1) for x in range(ncols)]

    # make side windows visible
    win4.set_visible(-50, 0, 0, -len(rows))
    win5.set_visible(-50, 0, 0, -len(rows))
    win1.set_visible(0, -50, -len(rows), 0)
    win3.set_visible(0, -50, -len(rows), 0)


    e = WindowEnsemble([win1, win2, win3], master=win2,
                       stackx=True, samew=True, tiex=True)
    e2 = WindowEnsemble([win4, win2, win5], master=win2,
                        stacky=True, sameh=True, tiey=True)

    win2.set_visible(0, -len(rows), len(cols), 0)
    # draw main window (checker board)
    vis = []
    for i in xrange(nrows):
        for j in xrange(ncols):
            if (i % 2) ^ (j % 2):
                vis.append(color(0, .2, i / float(nrows)))
            else:
                vis.append(color(1, i / float(nrows), 0))
            vis.append(quads(j, -i,
                           j, -i-1,
                           j+1, -i-1,
                           j+1, -i))
    win2.add_group(group(*vis))


    # draw side windows
    win4.add_group(make_labels(rows))
    win5.add_group(make_labels(rows))
    win1.add_group(make_labels(cols, True))
    win3.add_group(make_labels(cols, True))

summon.call(startup)
