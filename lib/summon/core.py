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

_VERTICES_CONSTRUCT = 1075
_COLOR_CONSTRUCT = 1076

_TRANSFORM_CONSTRUCT = 1077
_TRANSLATE_CONSTRUCT = 1078
_ROTATE_CONSTRUCT = 1079
_SCALE_CONSTRUCT = 1080
_FLIP_CONSTRUCT = 1081


class Construct:
    def __init__(self, constructid, code, options={}):
        self.constructid = constructid
        
        if "ref" in options and options["ref"]:
            # create a reference to an existing construct
            self.ptr = code[0]
            summon_core.incref_construct(code[0])
        else:
            # create a new construct
            try:
                self.ptr = summon_core.make_construct(constructid, code)
                #print "new", self.ptr
            except:
                self.ptr = None
                raise

    def __del__(self):
        #print "delete", self.ptr, self.constructid
        if self.ptr != None:
            # when python interface is GC also delete C++ construct
            summon_core.delete_construct(self.ptr)


    def __iter__(self):
        children = summon_core.get_construct_children(self.ptr)
        
        for i in xrange(0, len(children), 2):
            yield _make_ref(children[i], children[i+1])
    
    def get_contents(self):
        return summon_core.get_construct_contents(self.ptr)

    def get_children(self):
        return list(self)


class group (Construct):
    def __init__(self, *code, **options):
        Construct.__init__(self, _GROUP_CONSTRUCT, code, options)
    

class hotspot (Construct):
    def __init__(self, *code, **options):
        Construct.__init__(self, _HOTSPOT_CONSTRUCT, code, options)

#=============================================================================
# graphics

class graphic (Construct):
    pass
        

class points (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _POINTS_CONSTRUCT, code, options)

class lines (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _LINES_CONSTRUCT, code, options)

class line_strip (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _LINE_STRIP_CONSTRUCT, code, options)

class triangles (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TRIANGLES_CONSTRUCT, code, options)

class triangle_strip (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TRIANGLE_STRIP_CONSTRUCT, code, options)

class triangle_fan (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TRIANGLE_FAN_CONSTRUCT, code, options)

class quads (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _QUADS_CONSTRUCT, code, options)

class quad_strip (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _QUAD_STRIP_CONSTRUCT, code, options)

class polygon (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _POLYGON_CONSTRUCT, code, options)

class color_graphic (graphic):
    def __init__(self, *code, **options):
        Construct.__init__(self, _COLOR_CONSTRUCT, code, options)

def color(r, g, b, a=1.0):
    return (_COLOR_CONSTRUCT, r, g, b, a)

#class _color_ref (graphic):
#    def __init__(self, *code, **options):
#        Construct.__init__(self, _COLOR_CONSTRUCT, code, options)


#=============================================================================
# text

class text (Construct):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TEXT_CONSTRUCT, code, options)

class text_scale (text):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TEXT_SCALE_CONSTRUCT, code, options)

class text_clip (text):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TEXT_CLIP_CONSTRUCT, code, options)


#=============================================================================
# transforms

class transform (Construct):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TRANSFORM_CONSTRUCT, code, options)

class translate (transform):
    def __init__(self, *code, **options):
        Construct.__init__(self, _TRANSLATE_CONSTRUCT, code, options)

class rotate (transform):
    def __init__(self, *code, **options):
        Construct.__init__(self, _ROTATE_CONSTRUCT, code, options)

class scale (transform):
    def __init__(self, *code, **options):
        Construct.__init__(self, _SCALE_CONSTRUCT, code, options)

class flip (transform):
    def __init__(self, *code, **options):
        Construct.__init__(self, _FLIP_CONSTRUCT, code, options)


#=============================================================================
# global functions


def get_group_id(aGroup):
    return aGroup
    
def is_color(prim):
    return prim[0] == _COLOR_CONSTRUCT

def is_vertices(prim):
    return prim[0] == _VERTICES_CONSTRUCT
    

#=============================================================================
# private

_construct_table = {
    _GROUP_CONSTRUCT:          group,
    _HOTSPOT_CONSTRUCT:        hotspot,

    # graphics
    _POINTS_CONSTRUCT:         points,
    _LINES_CONSTRUCT:          lines,
    _LINE_STRIP_CONSTRUCT:     line_strip,
    _TRIANGLES_CONSTRUCT:      triangles,
    _TRIANGLE_STRIP_CONSTRUCT: triangle_strip,
    _TRIANGLE_FAN_CONSTRUCT:   triangle_fan,
    _QUADS_CONSTRUCT:          quads,
    _QUAD_STRIP_CONSTRUCT:     quad_strip,
    _POLYGON_CONSTRUCT:        polygon,
 
    # text
    _TEXT_CONSTRUCT:           text,
    _TEXT_SCALE_CONSTRUCT:     text_scale,
    _TEXT_CLIP_CONSTRUCT:      text_clip,
 
    _COLOR_CONSTRUCT:          color_graphic,

    # transforms
    _TRANSFORM_CONSTRUCT:      transform,
    _TRANSLATE_CONSTRUCT:      translate,
    _ROTATE_CONSTRUCT:         rotate,
    _SCALE_CONSTRUCT:          scale,
    _FLIP_CONSTRUCT:           flip
}


def _make_ref(constructid, ptr):
    return _construct_table[constructid](ptr, ref=True)



