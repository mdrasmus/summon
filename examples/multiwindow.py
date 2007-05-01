#!/usr/bin/python -i

from summon.core import *
from summon.multiwindow import *
import summon

win1 = summon.Window("window 1")
win1.add_group(group(lines(0, 0, 100,100)))

win2 = summon.Window("window 2")
win2.add_group(group(lines(0, 0, 100,100)))

win3 = summon.Window("window 3")
win3.add_group(group(lines(0, 0, 100,100)))


win1.set_size(400, 100)
win2.set_size(400, 100)
win3.set_size(400, 100)
pos = win1.get_position()
win2.set_position(pos[0], pos[1]+105)
win3.set_position(pos[0], pos[1]+216)



        

tie_windows([win1, win2, win3], tiex=True, pinx=True)
e = WindowEnsembl([win1, win2, win3])

summon.begin_updating()
