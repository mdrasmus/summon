"""
    SUMMON - core module

    This is the core summon module that is most often imported as
    from summon.core import *
    
"""

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
class SummonState (object):
    def __init__(self):
        self.current_window = None

        self.updateFuncs = []
        self.updateInterval = .5
        
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
        summon_core.del_model(model.id)
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
    """Return the currently active window"""
    return _summon_state.current_window


# All functions exported from summon_core 
#
# TODO: should only really need to export Contructs.  Everything else should
# be under summon.*


_summon_core_export = """\
color
color_contents
dynamic_group
dynamic_group_contents
flip
flip_contents
get_group_id
group
group_contents
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
points
points_contents
polygon
polygon_contents
quad_strip
quad_strip_contents
quads
quads_contents
rotate
rotate_contents
scale
scale_contents
text
text_clip
text_clip_contents
text_contents
text_scale
text_scale_contents
translate
translate_contents
triangle_fan
triangle_fan_contents
triangle_strip
triangle_strip_contents
triangles
triangles_contents
vertices
vertices_contents
""".split() + """\
assign_model
get_model
get_models
get_window
get_windows
new_groupid
new_model
new_window
redraw_call
timer_call
""".split()


_globals = globals()
for func in _summon_core_export:
    _globals[func] = summon_core.__dict__[func]


set_model = summon_core.set_model
set_window = summon_core.set_window
