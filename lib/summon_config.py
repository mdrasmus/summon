#
# Default configuration of Summon
#
# To make your own config file, place a config file called '.summon_config'
# in your HOME directory.  Alternatively, you can place 'summon_config.py' 
# in a directory in your python path.  Ensure that the dirctory appears BEFORE 
# .../summon/lib so that the new config file has precedence over this one.
# 
# A SUMMON configuration file must contain a function called 'config_window'
# that takes a summon Window as a single argument.  The config_window function
# must then initialize the window with its default configuration (mouse and key
# bindings, default background color, etc)
#


from summon.core import *

def config_window(win):
    # must wait until function call to import summon module
    import summon
    
    # clear all bindings
    win.clear_all_bindings()


    # scrolling
    win.set_binding(input_motion("left", "down"), "trans")
    win.set_binding(input_key("right"), win.trans_camera(20, 0))
    win.set_binding(input_key("left"), win.trans_camera(-20, 0))
    win.set_binding(input_key("up"), win.trans_camera(0, 20))
    win.set_binding(input_key("down"), win.trans_camera(0, -20))
    win.set_binding(input_key("right", "shift"), win.trans_camera(60, 0))
    win.set_binding(input_key("left", "shift"), win.trans_camera(-60, 0))
    win.set_binding(input_key("up", "shift"), win.trans_camera(0, 60))
    win.set_binding(input_key("down", "shift"), win.trans_camera(0, -60))


    # zooming
    win.set_binding(input_click("right", "down"), "focus")
    win.set_binding(input_click("right", "down", "ctrl"), "focus")
    win.set_binding(input_click("right", "down", "shift"), "focus")
    win.set_binding(input_motion("right", "down"), "zoom")
    win.set_binding(input_motion("right", "down", "ctrl"), "zoomx")
    win.set_binding(input_motion("right", "down", "shift"), "zoomy")

    win.set_binding(input_key("up", "shift"), win.zoom_camera(1, 1.2))
    win.set_binding(input_key("down", "shift"), win.zoom_camera(1, 1/1.2))
    win.set_binding(input_key("right", "shift"), win.zoom_camera(1.2, 1))
    win.set_binding(input_key("left", "shift"), win.zoom_camera(1/1.2, 1))
    win.set_binding(input_key("up", "ctrl"), win.zoom_camera(1.2))
    win.set_binding(input_key("down", "ctrl"), win.zoom_camera(1/1.2))


    # hotspot clicking
    win.set_binding(input_click("middle", "up"), "hotspot_click")
    win.set_binding(input_click("left", "up", "ctrl"), "hotspot_click")


    # menus
    win.set_menu(summon.SummonMenu(win))
    win.set_menu_button("left")
    win.set_binding(input_key("`"), win.activate_menu)
    win.add_binding(input_click("left", "up"), win.deactivate_menu)

    # misc
    win.set_binding(input_key("h"), win.home)
    win.set_binding(input_key("q"), win.close)
    win.set_binding(input_key("l", "ctrl"), win.toggle_aliasing)
    win.set_binding(input_key("x", "ctrl"), win.toggle_crosshair)
    win.set_binding(input_key("d", "ctrl"), win.duplicate)


    # add print screen bindings
    import summon.svg

    win.set_binding(input_key("p", "ctrl"), lambda: summon.svg.printScreen(win))
    win.set_binding(input_key("p", "ctrl", "shift"), lambda: summon.svg.printScreenPng(win))

