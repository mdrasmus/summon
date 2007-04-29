#
# This is the core summon module that is most often imported as
# from summon.core import *
#
# This should contain only summon_core and transparent python replacements
#

# python libs
import threading
import time

# SUMMON extension module
import summon_core


# start summon thread
summon_thread = threading.Thread(target=summon_core.summon_main_loop)
summon_thread.setDaemon(True)
summon_thread.start()

# wait for summon to startup
while summon_core.get_windows() == None: pass


#
# python state of SUMMON
#
class SummonState:
    def __init__(self):
        self.current_window = None
    
        self.antialias = True

        self.updateFuncs = []
        self.updateInterval = .5

        self.crosshair = False
        self.crosshair_color = None

_summon_state = SummonState()

def get_summon_state():
    return _summon_state

def get_summon_window():
    return _summon_state.current_window


# All functions exported from summon_core 
#
# TODO: should only really need to export Contructs.  Everything else should
# be under summon.*


def add_group(*groups):
    return summon_core.add_group(* groups)
    
"""
# future wrapper
def add_group(*groups):
    return summon_core.add_group(_summon_state.current_window.world.id, *groups)

"""

_summon_core_export = """\
assign_model
call_proc
clear_all_bindings
clear_binding
clear_groups
close_window
color
color_contents
del_model
dynamic_group
dynamic_group_contents
flip
flip_contents
focus
get_bgcolor
get_group
get_group_id
get_model
get_models
get_mouse_pos
get_root_id
get_visible
get_window
get_window_name
get_window_size
get_windows
group
group_contents
groupid
home
hotspot
hotspot_click
hotspot_contents
input_click
input_click_contents
input_key
input_key_contents
input_motion
input_motion_contents
insert_group
is_color
is_dynamic_group
is_flip
is_group
is_hotspot
is_input_click
is_input_key
is_input_motion
is_line_strip
is_lines
is_points
is_polygon
is_quad_strip
is_quads
is_rotate
is_scale
is_text
is_text_clip
is_text_scale
is_translate
is_triangle_fan
is_triangle_strip
is_triangles
is_vertices
line_strip
line_strip_contents
lines
lines_contents
list2group
new_groupid
new_model
new_window
points
points_contents
polygon
polygon_contents
quad_strip
quad_strip_contents
quads
quads_contents
quit
redraw_call
remove_group
replace_group
rotate
rotate_contents
scale
scale_contents
set_antialias
set_bgcolor
set_binding
set_crosshair_color
set_model
set_visible
set_window
set_window_name
set_window_size
show_crosshair
show_group
summon_main_loop
text
text_clip
text_clip_contents
text_contents
text_scale
text_scale_contents
timer_call
trans
translate
translate_contents
triangle_fan
triangle_fan_contents
triangle_strip
triangle_strip_contents
triangles
triangles_contents
version
vertices
vertices_contents
zoom
zoomx
zoomy
""".split()


_globals = globals()
for func in _summon_core_export:
    _globals[func] = summon_core.__dict__[func]








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


def reset_binding(input_obj, func):
    clear_binding(input_obj)
    set_binding(input_obj, func)
    
   

