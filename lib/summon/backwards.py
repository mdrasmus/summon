"""
    Backwards compatability module

    Currently this module provides replacements for the global functions 
    provided in SUMMON 1.6.1 and earlier.  It is recommended to use the new
    object-oriented functions.
"""


import summon_core
from summon.core import *

_summon_state = get_summon_state()

#=============================================================================
# these are wrappers to support the old global function interface
#


# model wrappers
def add_group(*groups):
    return summon_core.add_group(_summon_state.current_window.world.id, 
                                 *groups)
    
def insert_group(groupid, *groups):
    return summon_core.insert_group(_summon_state.current_window.world.id, 
                                    groupid, *groups)

def remove_group(*groupids):
    return summon_core.remove_group(_summon_state.current_window.world.id, 
                                    *groupids)

def replace_group(groupid, *groups):
    return summon_core.replace_group(_summon_state.current_window.world.id, 
                                     groupid, *groups)

def clear_groups():
    return summon_core.clear_groups(_summon_state.current_window.world.id)

def show_group(groupid, visible):
    return summon_core.show_group(_summon_state.current_window.world.id,
                                    groupid, visible)

def get_group(groupid):
    return summon_core.get_group(_summon_state.current_window.world.id,
                                 groupid)

def get_root_id():
    return summon_core.get_root_id(_summon_state.current_window.world.id)


# window function wrappers (for backward compatibility)
def set_window_size(w, h):
    return summon_core.set_window_size(_summon_state.current_window.winid,
                                       w, h)
def get_window_size():
    return summon_core.get_window_size(_summon_state.current_window.winid)

def set_bgcolor(*args):
    return summon_core.set_bgcolor(_summon_state.current_window.winid, *args)

def get_bgcolor(*args):
    return summon_core.get_bgcolor(_summon_state.current_window.winid, *args)

def set_visible(*args):
    return summon_core.set_visible(_summon_state.current_window.winid, *args)

def get_visible(*args):
    return summon_core.get_visible(_summon_state.current_window.winid, *args)

def home():
    return summon_core.home(_summon_state.current_window.winid)

def set_antialias(enabled):
    return _summon_state.current_window.set_antialias(enabled)

def show_crosshair(enabled):
    return _summon_state.current_window.show_crosshair(enabled)

def set_crosshair_color(*args):
    return _summon_state.current_window.set_crosshair_color(*args)

# basic view
def trans(x, y):
    summon_core.trans(_summon_state.current_window.winid, x, y)

def zoom(x, y):
    summon_core.zoom(_summon_state.current_window.winid, x, y)

def focus(x, y):
    summon_core.focus(_summon_state.current_window.winid, x, y)

def zoomx(x):
    summon_core.zoomx(_summon_state.current_window.winid, x)

def zoomy(y):
    summon_core.zoomy(_summon_state.current_window.winid, y)


# controller wrappers
def clear_binding(input_obj):
    summon_core.clear_binding(_summon_state.current_window.winid, input_obj)

def clear_all_bindings():
    summon_core.clear_all_bindings(_summon_state.current_window.winid)

def get_mouse_pos(coord):
    return summon_core.get_mouse_pos(_summon_state.current_window.winid, coord)

def set_binding(input_obj, func):
    winid = _summon_state.current_window.winid
    summon_core.clear_binding(winid, input_obj)
    summon_core.set_binding(winid, input_obj, func)
reset_binding = set_binding

def add_binding(input_obj, func):
    winid = _summon_state.current_window.winid
    summon_core.set_binding(winid, input_obj, func)
