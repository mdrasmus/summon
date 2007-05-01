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
__summon_thread = threading.Thread(target=summon_core.summon_main_loop)
__summon_thread.setDaemon(True)
__summon_thread.start()

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
        
        self.windows = {}
        self.models = {}
    
    
    def add_window(self, win):
        self.windows[win.winid] = win
    
    def remove_window(self, win):
        if win.winid in self.windows:
            del self.windows[win.winid]
    
    def get_window(self, winid):
        if winid in self.windows:
            return self.windows[winid]
        else:
            return None
    
    def add_model(self, model):
        self.models[model.id] = model
    
    def remove_model(self, model):
        if model.id in self.models:
            del self.models[model.id]

    def get_model(self, modelid):
        if modelid in self.models:
            return self.models[modelid]
        else:
            return None



_summon_state = SummonState()

def get_summon_state():
    return _summon_state

def get_summon_window():
    return _summon_state.current_window


# All functions exported from summon_core 
#
# TODO: should only really need to export Contructs.  Everything else should
# be under summon.*


#
# these are wrappers around the old global function interface
#

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


#
# view wrappers
#

def set_window_size(w, h):
    return summon_core.set_window_size(_summon_state.current_window.winid,
                                       w, h)


def get_window_size():
    return summon_core.get_window_size(_summon_state.current_window.winid)


_summon_core_export = """\
assign_model
call_proc
clear_all_bindings
clear_binding
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
get_group_id
get_model
get_models
get_mouse_pos
get_visible
get_window
get_window_name
get_windows
group
group_contents
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
show_crosshair
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


add_binding = set_binding
def set_binding(input_obj, func):
    clear_binding(input_obj)
    add_binding(input_obj, func)
reset_binding = set_binding
