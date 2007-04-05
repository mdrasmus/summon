#
# This is the core summon module that is most often imported as
# from summon.core import *
#
# This should contain only summon_core and transparent python replacements
#

import threading
import time

from summon_core import *


# start summon thread
summon_thread = threading.Thread(target=summon_main_loop)
summon_thread.setDaemon(True)
summon_thread.start()

# wait for it to be ready
while get_windows() == None: pass


#
# state of SUMMON
#
class State:
    def __init__(self):
        self.antialias = True

        self.updateFuncs = []
        self.updateInterval = .5

        self.crosshair = False
        self.crosshair_color = None

_state = State()

def get_summon_state():
    return _state

#
# common functions needed for key bindings
#
def trans_camera(x, y):
    def func():
        trans(x, y)
    return func

def zoom_camera(factor, factor2=None):
    if factor2 == None:
        factor2 = factor

    def func():
        w, h = get_window_size()
        focus(w/2, h/2)
        zoom(factor, factor2)
    return func

def toggle_aliasing():
    _state.antialias = not _state.antialias
    set_antialias(_state.antialias)

def toggle_crosshair():
    _state.crosshair = not _state.crosshair
    show_crosshair(_state.crosshair)
    
    if _state.crosshair_color == None:
        col = get_bgcolor()
        set_crosshair_color(1-col[0], 1-col[1], 1-col[2], 1)

_set_crosshair_color = set_crosshair_color
def set_crosshair_color(r, g, b, a=1):
    _state.crosshair_color = (r, g, b, a)
    _set_crosshair_color(r, g, b, a)


_show_crosshair = show_crosshair
def show_crosshair(enabled):
    _state.crosshair = enabled
    _show_crosshair(enabled)


def reset_binding(input_obj, func):
    clear_binding(input_obj)
    set_binding(input_obj, func)
