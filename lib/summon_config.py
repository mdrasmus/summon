#
# Default configuration of Summon
#
# To make your own config file, place a config file called 'summon_config.py' 
# in a directory in your python path.  Ensure that the dirctory appears BEFORE 
# .../summon/lib so that the new config file has precedence over this one.
#

from summon import *
from summonlib import *
import summonlib.svg

# scrolling
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

# hotspot clicking
set_binding(input_click("middle", "up"), "hotspot_click")
set_binding(input_click("left", "up", "ctrl"), "hotspot_click")

# misc
set_binding(input_key("h"), "home")
set_binding(input_key("q"), "quit")
set_binding(input_key("l", "ctrl"), toggle_aliasing)
set_binding(input_key("d", "ctrl"), dupWindow)

