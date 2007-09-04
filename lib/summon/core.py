"""
    SUMMON - core module

    This is the core summon module that provides the most common primitives for
    creating visualizations in SUMMON.  Since these primitives are used so often
    this modules is most often imported as:
        from summon.core import *
    
"""

# python libs
import threading
import time
import atexit

# SUMMON extension module
import summon_core




# start summon thread
_summon_thread = threading.Thread(target=summon_core.summon_main_loop)
_summon_thread.setDaemon(True)
_summon_thread.start()

# register a function for clean shutdown
atexit.register(lambda: summon_core.summon_shutdown)

# wait for summon to startup
while summon_core.get_windows() == None: time.sleep(.05)



#=============================================================================
# constuct IDs

_i = iter(xrange(2000, 2000+200))

_GROUP_CONSTRUCT = _i.next()
_i.next()
_HOTSPOT_CONSTRUCT = _i.next()

_POINTS_CONSTRUCT = _i.next()
_LINES_CONSTRUCT = _i.next()
_LINE_STRIP_CONSTRUCT = _i.next()
_TRIANGLES_CONSTRUCT = _i.next()
_TRIANGLE_STRIP_CONSTRUCT = _i.next()
_TRIANGLE_FAN_CONSTRUCT = _i.next()
_QUADS_CONSTRUCT = _i.next()
_QUAD_STRIP_CONSTRUCT = _i.next()
_POLYGON_CONSTRUCT = _i.next()
_TEXT_CONSTRUCT = _i.next()
_TEXT_SCALE_CONSTRUCT = _i.next()
_TEXT_CLIP_CONSTRUCT = _i.next()

_VERTICES_CONSTRUCT = _i.next()
_COLOR_CONSTRUCT = _i.next()

_TRANSFORM_CONSTRUCT = _i.next()
_TRANSLATE_CONSTRUCT = _i.next()
_ROTATE_CONSTRUCT = _i.next()
_SCALE_CONSTRUCT = _i.next()
_FLIP_CONSTRUCT = _i.next()
_i.next()
_i.next()
   
# inputs
_INPUT_KEY_CONSTRUCT = _i.next()
_INPUT_CLICK_CONSTRUCT = _i.next()
_INPUT_MOTION_CONSTRUCT = _i.next()


#=============================================================================
# Element classes

class Element:
    """This is the abstract base class of all graphical elements in SUMMON.
       Graphical elements are organized as follows:
       
       Element: base class
            - group
            - hotspot
            - text: base class of all text elements
                - text_scale
                - text_clip
            - etc...
            
            Graphic: lines and polygons
                - lines
                - quads
                - etc...
            
            Transform: base class for all transformations
                - translate
                - rotate
                - scale
                - flip
    """
        

    def __init__(self, elementid, code, options={}):
        """Create a new element from the SUMMON extension module.
           This class is typically not instantiated directly by user code.
        """
        self.elementid = elementid
        
        if "ref" in options and options["ref"]:
            # create a reference to an existing construct
            self.ptr = code[0]
            summon_core.incref_construct(code[0])
        else:
            # create a new construct
            try:
                self.ptr = summon_core.make_construct(elementid, code)
                #print "new", self.ptr
            except:
                self.ptr = None
                raise

    def __del__(self):
        #print "delete", self.ptr, self.elementid
        if self.ptr != None:
            # when python interface is GC also delete C++ construct
            summon_core.delete_construct(self.ptr)

    def __iter__(self):
        """Iterates through this element's child elements"""
        children = summon_core.get_construct_children(self.ptr)
        
        for i in xrange(0, len(children), 2):
            yield _make_ref(children[i], children[i+1])

    def __getitem__(self, i):
        return list(self)[i]
    
    def get_contents(self):
        """Returns the a tuple in a format specific to the content 
           of this element"""
        return summon_core.get_construct_contents(self.ptr)

    def get_children(self):
        """Returns the children of this element as a list"""
        return list(self)
    
    def append(self, aGroup):
        summon_core.append_group(self.ptr, aGroup)
        return aGroup
    
    def remove(self, *groups):
        summon_core.remove_group2(self.ptr, *[x.ptr for x in groups])

    def replace(self, oldchild, newchild):
        pass
        
    def set_visible(self, vis):
        pass
    
    def get_bounding(self):
        pass


class group (Element):
    """Groups together several graphical elements into one"""
    def __init__(self, *elements, **options):
        Element.__init__(self, _GROUP_CONSTRUCT, elements, options)
    

class hotspot (Element):
    """Designates a region of the screen to react to mouse clicks.
    
       When a mouse click (default: middle click) occurs within the specified
       rectangular region the given function will be called."""
    def __init__(self, *args, **options):
        """kind - must be the string "click" (more options in future versions)
           x1   - 1st x-coordinate of rectangular region
           y1   - 1st y-coordinate of rectangular region
           x2   - 2nd x-coordinate of rectangular region
           y2   - 2nd y-coordinate of rectangular region
           func - callback function of no arguments
        """
        Element.__init__(self, _HOTSPOT_CONSTRUCT, 
                         args, options)


#=============================================================================
# graphics

class Graphic (Element):
    """This the abstract base class of all line and polygon graphical elements.
       Every graphic consists of only vertices and color primitives.
    """
    pass
        

class points (Graphic):
    """A graphic that draws one or more points"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        
            ex: points(100, 200)  
            # a single point at (100,200)
        
            ex: points(color(1, 0, 0), 0, 0, 100, 200,
                       color(0, 0, 1), 100, 100, 300, 400)
            # four points, two red the other two blue
        """
        Element.__init__(self, _POINTS_CONSTRUCT, primitives, options)

class lines (Graphic):
    """A graphic that draws one or more lines"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        
            ex: lines(0, 0, 100, 200)  
            # a single line from (0,0) to (100,200)
        
            ex: lines(color(1, 0, 0), 0, 0, 100, 200,
                      color(0, 0, 1), 100, 100, 300, 400)
            # two lines, one red the other blue
        """
        Element.__init__(self, _LINES_CONSTRUCT, primitives, options)

class line_strip (Graphic):
    """A graphic that draws one or more lines that are connected end to end"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        
            ex: line_strip(0, 0, 100, 200, 300, 400)  
            # two lines, one from (0,0) to (100,200)
            # the other from (100, 200) to (300, 400)
        """    
        Element.__init__(self, _LINE_STRIP_CONSTRUCT, primitives, options)

class triangles (Graphic):
    """A graphic that draws one or more triangles"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        
            ex: triangles(0, 0, 100, 200, 300, 400)              
            # a trinagle with vertices (0,0), (100,200) and (300, 400)
            
            ex: triangles(0, 0, 100, 200, 300, 400,
                          400, 400, 500, 400, 400, 500)              
            # two triangles
        """    
        Element.__init__(self, _TRIANGLES_CONSTRUCT, primitives, options)

class triangle_strip (Graphic):
    """A graphic that draws one or more triangles that align edge to edge in 
       a long strip"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        
            ex: triangle_strip(0, 0, 0, 200, 200, 0, 200, 200)
            # two triangles with vertices (0,0),(0,200),(200,0) and
            # another with vertices (0,200),(200,0),(200,200)
        """
        Element.__init__(self, _TRIANGLE_STRIP_CONSTRUCT, primitives, options)

class triangle_fan (Graphic):
    """A graphic that draws one or more triangles in a fan shape"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        """
        Element.__init__(self, _TRIANGLE_FAN_CONSTRUCT, primitives, options)

class quads (Graphic):
    """A graphic that draws one or more quadrilaterals"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        
           ex: quads(50,0, 50,70, 60,70, 60,0)
           # a quadrilateral with vertices (50,0), (50,70), (60,70), (60,0)
        """    
        Element.__init__(self, _QUADS_CONSTRUCT, primitives, options)

class quad_strip (Graphic):
    """A graphic that draws one or more quadrilaterals that align edge to edge
       in a long strip"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        """    
        Element.__init__(self, _QUAD_STRIP_CONSTRUCT, primitives, options)

class polygon (Graphic):
    """A graphic that draws one polygon with an arbitrary number of vertices"""
    def __init__(self, *primitives, **options):
        """*primitives - a series of coordinates and color primitives
        """
        Element.__init__(self, _POLYGON_CONSTRUCT, primitives, options)


#=============================================================================
# primitives

def color(red, green, blue, alpha=1.0):
    """A primitive that sets the current color
    
       red   - a value from 0 to 1
       green - a value from 0 to 1
       blue  - a value from 0 to 1
       alpha - a value from 0 to 1 (opacity)
    """
    return (_COLOR_CONSTRUCT, red, green, blue, alpha)
    
def is_color(prim):
    """test whether an object is a color primitive"""
    return prim[0] == _COLOR_CONSTRUCT

def is_vertices(prim):
    """test whether an object is a vertices primitive"""
    return prim[0] == _VERTICES_CONSTRUCT
   

class color_graphic (Graphic):
    """A graphic that sets the current color
       NOTE: this object should not be used directly, see: color(r, g, b, a)
    """
    def __init__(self, *args, **options):
        Element.__init__(self, _COLOR_CONSTRUCT, args, options)


#=============================================================================
# text


class text (Element):
    """A bitmap text element that automatically hides if it cannot fit within 
       its bounding box"""
    def __init__(self, *args, **options):
        """txt        - text to display
           x1         - 1st x-coordinate of bounding box
           y1         - 1st y-coordinate of bounding box
           x2         - 2nd x-coordinate of bounding box
           y2         - 2nd y-coordinate of bounding box
           *justified - one or more of the following strings, indicating how
                        to justify the text within the bounding box
                'left' 'center' 'right' 
                'bottom' 'middle' 'top'
        """
        Element.__init__(self, _TEXT_CONSTRUCT, 
                                 args, 
                                 options)

class text_scale (text):
    """A vector graphics text element"""
    def __init__(self, *args, **options):
        """txt        - text to display
           x1         - 1st x-coordinate of bounding box
           y1         - 1st y-coordinate of bounding box
           x2         - 2nd x-coordinate of bounding box
           y2         - 2nd y-coordinate of bounding box
           *justified - one or more of the following strings, indicating how
                        to justify the text within the bounding box
                'left' 'center' 'right' 
                'bottom' 'middle' 'top'
        """
        Element.__init__(self, _TEXT_SCALE_CONSTRUCT, 
                                 args, 
                                 options)

class text_clip (text):
    """A vector graphics text element that has a minimum and maximum height"""
    def __init__(self, *args, 
                       **options):
        """txt        - text to display
           x1         - 1st x-coordinate of bounding box
           y1         - 1st y-coordinate of bounding box
           x2         - 2nd x-coordinate of bounding box
           y2         - 2nd y-coordinate of bounding box
           minheight  - minimum on-screen height (pixels)
           maxheight  - maximum on-screen height (pixels)
           *justified - one or more of the following strings, indicating how
                        to justify the text within the bounding box
                'left' 'center' 'right' 
                'bottom' 'middle' 'top'
        """    
        Element.__init__(self, _TEXT_CLIP_CONSTRUCT, 
                  args, 
                  options)


#=============================================================================
# transforms

class Transform (Element):
    """Base class for all transformation elements"""
    def __init__(self, *args, **options):
        Element.__init__(self, _TRANSFORM_CONSTRUCT, args, options)

class translate (Transform):
    """Translates the containing elements"""
    def __init__(self, *args, **options):
        """x         - units along the x-axis to translate
           y         - units along the y-axis to translate
           *elements - one or more elements to translate
        """
        Element.__init__(self, _TRANSLATE_CONSTRUCT, 
                         args, options)

class rotate (Transform):
    """Rotates the containing elements"""
    def __init__(self, *args, **options):
        """angle     - angle in degrees (-360, 360) to rotate (clock-wise)
           *elements - one or more elements to rotate
        """
        Element.__init__(self, _ROTATE_CONSTRUCT, 
                         args, options)

class scale (Transform):
    """Scales the containing elements"""
    def __init__(self, *args, **options):
        """scalex    - factor of scaling along x-axis
           scaley    - factor of scaling along y-axis
           *elements - one or more elements to scale
        """
        Element.__init__(self, _SCALE_CONSTRUCT, 
                         args, options)

class flip (Transform):
    """Flips the containing elements over a line (0,0)-(x,y)"""
    def __init__(self, *args, **options):
        """x         - x-coordinate of line
           y         - y-coordinate of line
           *elements - one or more elements to flips
        """
        Element.__init__(self, _FLIP_CONSTRUCT, 
                         args, options)

 
#=============================================================================
# input constructs

def input_key(key, *mods):
    """Specifies a keyboard input
        
       key   - key press
       *mods - zero or many modifier keys: 'shift', 'ctrl', 'alt'
    """
    return _tuple(_INPUT_KEY_CONSTRUCT, key, *mods)

def is_input_key(input_construct):
    """Returns true if argument is an input_key"""
    return input_construct[0] == _INPUT_KEY_CONSTRUCT

def input_key_contents(input_construct):
    """Returns the contents of an input_key object"""
    return input_construct[1:]



def input_click(button, state, *mods):
    """Specifies a mouse click input
    
       button  - mouse button: 'left'|'middle'|'right', 
       state   - state of button: 'up'|'down',
       *mods   - zero or many modifier keys: 'shift', 'ctrl', 'alt'
    """
    return _tuple(_INPUT_CLICK_CONSTRUCT, button, state, *mods)

def is_input_click(input_construct):
    """Returns true if argument is an input_click"""
    return input_construct[0] == _INPUT_CLICK_CONSTRUCT

def input_click_contents(input_construct):
    """Returns the contents of an input_click object"""
    return input_construct[1:]



def input_motion(button, state, *mods):
    """Specifies a mouse motion input

       button  - mouse button: 'left'|'middle'|'right', 
       state   - state of button: 'up'|'down',
       *mods   - zero or many modifier keys: 'shift', 'ctrl', 'alt'    
    """
    return _tuple(_INPUT_MOTION_CONSTRUCT, button, state, *mods)

def is_input_motion(input_construct):
    """Returns true if argument is an input_motion"""
    return input_construct[0] == _INPUT_MOTION_CONSTRUCT

def input_motion_contents(input_construct):
    """Returns the contents of an input_motion object"""
    return input_construct[1:]



#=============================================================================
# global functions


def get_group_id(aGroup):
    """DEPRECATED: group IDs are no longer needed"""
    return aGroup




#=============================================================================
# private

_element_table = {
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
    _TRANSFORM_CONSTRUCT:      Transform,
    _TRANSLATE_CONSTRUCT:      translate,
    _ROTATE_CONSTRUCT:         rotate,
    _SCALE_CONSTRUCT:          scale,
    _FLIP_CONSTRUCT:           flip
}


def _make_ref(elementid, ptr):
    return _element_table[elementid](ptr, ref=True)


def _tuple(*args):
    return args


