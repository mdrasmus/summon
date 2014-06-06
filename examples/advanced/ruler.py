
from summon.core import *
import summon
from summon import hud
reload(hud)


# create a new window
win = summon.Window("01_basics")
win.add_group(lines(0,0, 30,40))
win.add_group(group(color(0, 0, 1), 
                    quads(50,0, 50,70, 60,70, 60,0),
                    quads(65,0, 65,70, 75,70, 75,0)))
win.add_group(quads(color(1,0,0), 100, 0,
                    color(0,1,0), 100, 70,
                    color(0,0,1), 140, 60,
                    color(1,1,1), 140, 0))
win.home()

win.set_bgcolor(0,0,0)

ruler = hud.RulerHud(win, width=100)


