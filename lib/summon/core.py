"""
    SUMMON - core module

    This is the core summon module that is most often imported as:
        from summon.core import *
    
"""

#
# This should contain only summon_core and transparent python replacements
#

# python libs
import threading
import time
import atexit

# SUMMON extension module
import summon_core




# start summon thread
__summon_thread = threading.Thread(target=summon_core.summon_main_loop)
__summon_thread.setDaemon(True)
__summon_thread.start()

# register a function for clean shutdown
atexit.register(lambda: summon_core.summon_shutdown)

# wait for summon to startup
while summon_core.get_windows() == None: time.sleep(.05)




# All functions exported from summon_core 
#
# TODO: should only really need to export Contructs.  Everything else should
# be under summon.*


_summon_constructs = """\
color_contents
dynamic_group
dynamic_group_contents
flip_contents
group_contents
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
line_strip_contents
lines_contents
points_contents
polygon_contents
quad_strip_contents
quads_contents
rotate_contents
scale_contents
text_clip_contents
text_contents
text_scale_contents
translate_contents
triangle_fan_contents
triangle_strip_contents
triangles_contents
vertices
vertices_contents
""".split()


# Note: these will be removed some day
# only timer_call is necessary right now
#_summon_funcs = """\
#""".split()


_globals = globals()
for func in _summon_constructs: # + _summon_funcs:
    _globals[func] = summon_core.__dict__[func]



#=============================================================================
# constuct IDs

_GROUP_CONSTRUCT = 1060
_HOTSPOT_CONSTRUCT = 1062

_POINTS_CONSTRUCT = 1063
_LINES_CONSTRUCT = 1064
_LINE_STRIP_CONSTRUCT = 1065
_TRIANGLES_CONSTRUCT = 1066
_TRIANGLE_STRIP_CONSTRUCT = 1067
_TRIANGLE_FAN_CONSTRUCT = 1068
_QUADS_CONSTRUCT = 1069
_QUAD_STRIP_CONSTRUCT = 1070
_POLYGON_CONSTRUCT = 1071
_TEXT_CONSTRUCT = 1072
_TEXT_SCALE_CONSTRUCT = 1073
_TEXT_CLIP_CONSTRUCT = 1074

_COLOR_CONSTRUCT = 1076

_TRANSLATE_CONSTRUCT = 1078
_ROTATE_CONSTRUCT = 1079
_SCALE_CONSTRUCT = 1080
_FLIP_CONSTRUCT = 1081


class Construct:
    def __init__(self, constructid, code):
        self.constructid = constructid
        self.dels = 0
        try:
            self.ptr = summon_core.make_construct(constructid, code)
        except:
            self.ptr = None
            raise

    def __del__(self):
        if self.ptr != None:
            summon_core.delete_construct(self.ptr)

class group (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _GROUP_CONSTRUCT, code)

class group_dummy (group):
    def __init__(self, ptr):
        self.constructid = _GROUP_CONSTRUCT
        self.ptr = ptr
    
    def __del__(self):
        self.ptr = None


class hotspot (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _HOTSPOT_CONSTRUCT, code)

#=============================================================================
# graphics

class points (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _POINTS_CONSTRUCT, code)

class lines (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _LINES_CONSTRUCT, code)

class line_strip (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _LINE_STRIP_CONSTRUCT, code)

class triangles (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _TRIANGLES_CONSTRUCT, code)

class triangle_strip (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _TRIANGLE_STRIP_CONSTRUCT, code)

class quads (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _QUADS_CONSTRUCT, code)

class quad_strip (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _QUAD_STRIP_CONSTRUCT, code)

class polygon (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _POLYGON_CONSTRUCT, code)

def color(r, g, b, a=1.0):
    return (_COLOR_CONSTRUCT, r, g, b, a)


#=============================================================================
# text

class text (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _TEXT_CONSTRUCT, code)

class text_scale (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _TEXT_SCALE_CONSTRUCT, code)

class text_clip (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _TEXT_CLIP_CONSTRUCT, code)


#=============================================================================
# transforms

class translate (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _TRANSLATE_CONSTRUCT, code)

class rotate (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _ROTATE_CONSTRUCT, code)

class scale (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _SCALE_CONSTRUCT, code)

class flip (Construct):
    def __init__(self, *code):
        Construct.__init__(self, _FLIP_CONSTRUCT, code)


#=============================================================================
# global functions

def get_group_id(aGroup):
    return aGroup

