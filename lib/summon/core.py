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




# All functions exported from summon_core 
#
# TODO: should only really need to export Contructs.  Everything else should
# be under summon.*


_summon_constructs = """\
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
""".split()


# Note: these will be removed some day
# only timer_call is necessary right now
_summon_funcs = """\
assign_model
get_models
get_windows
new_groupid
new_model
new_window
redraw_call
timer_call
""".split()


_globals = globals()
for func in _summon_constructs + _summon_funcs:
    _globals[func] = summon_core.__dict__[func]


# removed commands:
#   set_model
#   get_model
#   set_window
#   get_window
