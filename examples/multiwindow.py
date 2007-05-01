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

win4 = summon.Window("window 4")
win4.add_group(group(lines(0, 0, 100,100)))

win5 = summon.Window("window 5")
win5.add_group(group(lines(0, 0, 100,100)))


win1.set_size(400, 100)
win2.set_size(400, 400)
win3.set_size(400, 100)
win4.set_size(100, 400)
win5.set_size(100, 400)

#pos = win1.get_position()
#win2.set_position(pos[0], pos[1]+105)
#win3.set_position(pos[0], pos[1]+216)



       
tie_windows([win1, win2, win3], tiex=True, pinx=True)
tie_windows([win4, win2, win5], tiey=True, piny=True)
e = WindowEnsembl([win1, win2, win3], stackx=True, samew=True)
e = WindowEnsembl([win4, win2, win5], stacky=True, sameh=True)

summon.begin_updating()
