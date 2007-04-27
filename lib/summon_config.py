#
# Default configuration of Summon
#
# To make your own config file, place a config file called '.summon_config'
# in your HOME directory.  Alternatively, you can place 'summon_config.py' 
# in a directory in your python path.  Ensure that the dirctory appears BEFORE 
# .../summon/lib so that the new config file has precedence over this one.
# 
#

import sys
from summon.core import *
import summon


win = get_summon_window()
print "window", win

clear_all_bindings()

def _set_and_call(win, func):
    win.activate()
    func()


# scrolling
set_binding(input_motion("left", "down"), "trans")
set_binding(input_key("right"), trans_camera(20, 0))
set_binding(input_key("left"), trans_camera(-20, 0))
set_binding(input_key("up"), trans_camera(0, 20))
set_binding(input_key("down"), trans_camera(0, -20))
set_binding(input_key("right", "shift"), trans_camera(60, 0))
set_binding(input_key("left", "shift"), trans_camera(-60, 0))
set_binding(input_key("up", "shift"), trans_camera(0, 60))
set_binding(input_key("down", "shift"), trans_camera(0, -60))
set_binding(input_key("Z", "shift"), zoom_camera(1.2))
set_binding(input_key("z"), zoom_camera(.8333))


# zooming
set_binding(input_click("right", "down"), "focus")
set_binding(input_click("right", "down", "ctrl"), "focus")
set_binding(input_click("right", "down", "shift"), "focus")
set_binding(input_motion("right", "down"), "zoom")
set_binding(input_motion("right", "down", "ctrl"), "zoomx")
set_binding(input_motion("right", "down", "shift"), "zoomy")


# hotspot clicking
set_binding(input_click("middle", "up"), "hotspot_click")
set_binding(input_click("left", "up", "ctrl"), "hotspot_click")


# misc
set_binding(input_key("h"), "home")
set_binding(input_key("q"), lambda: _set_and_call(win, close_window))
set_binding(input_key("l", "ctrl"), lambda: _set_and_call(win, toggle_aliasing))
set_binding(input_key("x", "ctrl"), lambda: _set_and_call(win, toggle_crosshair))
set_binding(input_key("d", "ctrl"), lambda: summon.dupWindow(win.winid))


# add print screen bindings
import summon.svg

set_binding(input_key("p", "ctrl"), 
            lambda: _set_and_call(win, summon.svg.printScreen))
set_binding(input_key("p", "ctrl", "shift"), 
            lambda: _set_and_call(win, summon.svg.printScreenPng))

