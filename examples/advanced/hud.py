#!/usr/bin/env python-i


from summon.core import *
import summon
from summon import shapes, hud


win = summon.Window("screen_model")

win.add_group(group(color(0, 1, 0), lines(0, 0, 100, 100)))

def func(name):
    print name

bar = hud.SideBar(win, width=200)
bar.add_item(hud.MenuItem("My Func1", lambda: func(1)))
bar.add_item(hud.MenuItem("My Func2", lambda: func(2)))
bar.add_item(hud.MenuItem("My Func3", lambda: func(3), 
                          button_color=(.7, .9,.7)))
bar.add_item(hud.MenuItem("My Func4", lambda: func(4), 
                          button_color=(.9, .7,.7)))
bar.show()


win.set_size(400,400)
win.set_visible(0,0, 300, 300)

