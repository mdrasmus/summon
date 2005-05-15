#
# Default setup of Visdraw
#

from summon import *
from __init__ import *
import svg

set_binding(input_key("h"), "home")
set_binding(input_key("q"), "quit")
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
set_binding(input_click("middle", "up"), "hotspot_click")
set_binding(input_click("left", "up", "ctrl"), "hotspot_click")
set_binding(input_key("l", "ctrl"), toggle_aliasing)
set_binding(input_key("d", "ctrl"), dupWindow)

