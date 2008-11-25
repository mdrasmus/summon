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
import math

# SUMMON extension module
import summon_core
import vector


#=============================================================================
# constuct IDs

_i = iter(xrange(2000))

_GROUP_CONSTRUCT = _i.next()
_CUSTOM_GROUP_CONSTRUCT = _i.next()
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
_ZOOM_CLAMP_CONSTRUCT = _i.next()
_i.next()
_i.next()
   
# inputs
_INPUT_KEY_CONSTRUCT = _i.next()
_INPUT_CLICK_CONSTRUCT = _i.next()
_INPUT_MOTION_CONSTRUCT = _i.next()


# hotspot events
_int2hotspot_event = [
    "click",
    "over", # TODO: future use
    "out",  # TODO: future use
    "drag", 
    "drag_start",
    "drag_stop"
]

_INF = float("1e1000")

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
           
           NOTE: To create a reference to an existing element include the
           optional argument options["ref"] = ptr, where 'ptr' is a valid
           element pointer of type int.  The 'code' can be empty.  
           
           NOTE: all subclasses of Element must allow initiation with 
           no positional arguments and the optional argument 'ref'.  This call
           is used by _make_ref().
        """
        self.elementid = elementid
        
        if "ref" in options:
            # create a reference to an existing element
            self.ptr = options["ref"]
            summon_core.incref_element(self.ptr)
        else:
            # create a new element
            try:
                self.ptr = summon_core.make_element(elementid, code)
                #print "new", self.ptr
            except:
                self.ptr = None
                raise

    def __del__(self):
        #print "delete", self.ptr, self.elementid
        if self.ptr != None:
            # when python interface is GC also delete C++ element
            summon_core.delete_element(self.ptr)

    def __iter__(self):
        """Iterates through this element's child elements"""
        children = summon_core.get_element_children(self.ptr)
        
        for i in xrange(0, len(children), 2):
            yield _make_ref(children[i], children[i+1])

    def __getitem__(self, i):
        """Gets the i'th child of an element"""
        if isinstance(i, int):
            children = summon_core.get_element_children(self.ptr)
            if i < 0 or 2*i+1 >= len(children):
                raise IndexError("child index out of range")
        
            return _make_ref(children[2*i], children[2*i+1])
        else:
            return list(self)[i]
    
    def __eq__(self, other):
        return self.ptr == hash(other)
    
    def __ne__(self, other):
        return self.ptr != hash(other)
    
    def __cmp__(self, other):
        return cmp(self.ptr, hash(other))
    
    def __hash__(self):
        return self.ptr
    
    def get(self):
        """Returns the content of an element as a tuple in a format specific 
           to the particular element type"""
        return summon_core.get_element_contents(self.ptr)

    def set(self, *args):
        """Sets the contents of the element
           The argument format is specific to the particular element type"""
        return summon_core.set_contents(self.ptr, *args)

    def get_children(self):
        """Returns the children of this element as a list"""
        return list(self)
    
    def get_parent(self):
        """Returns the parent of this element or None if no parent exists"""
        elementid, parent = summon_core.get_element_parent(self.ptr)
        
        if parent == 0:
            return None
        else:
            return _make_ref(elementid, parent)
        
    
    def append(self, element):
        """Append a drawing element to the children of this element"""
        summon_core.append_group(self.ptr, element)
        return element
    
    def remove(self, *elements):
        """Remove drawing elements from the children of this element"""
        summon_core.remove_group2(self.ptr, *[x.ptr for x in elements])

    def remove_self(self):
        """Remove the element from its own parent"""
        elementid, parent = summon_core.get_element_parent(self.ptr)
        
        if parent == 0:
            raise Exception("element has no parent, cannot remove")
        else:
            summon_core.remove_group2(parent, self.ptr)
    
    def replace(self, oldchild, newchild):
        """Replace a child element 'oldchild' with a new child 'newchild'"""
        summon_core.replace_group2(self.ptr, oldchild.ptr, newchild)
        return newchild
    
    def replace_self(self, newelm):
        """Replace the element with a new element"""
        elementid, parent = summon_core.get_element_parent(self.ptr)
        
        if parent == 0:
            raise Exception("element has no parent, cannot replace")
        else:
            summon_core.replace_group2(parent, self.ptr, newelm)
            return newelm
    
    def set_visible(self, vis):
        """Set the visibility of an element"""
        summon_core.show_group2(self.ptr, vis)
    
    def get_visible(self):
        """NOT implemented yet"""
        pass
    
    def get_bounding(self):
        """Gets the bounding box of an element and all of its children"""
        return summon_core.get_bounding2(self.ptr)


class group (Element):
    """Groups together several graphical elements into one"""
    def __init__(self, *elements, **options):
        Element.__init__(self, _GROUP_CONSTRUCT, elements, options)


class custom_group (Element):
    """Acts like a group but allows safe subclassing"""
    def __init__(self, *elements, **options):
        Element.__init__(self, _CUSTOM_GROUP_CONSTRUCT, (self, elements), options)


#=============================================================================
# hotspots - mouse interaction

class hotspot (Element):
    """Designates a region of the screen to react to mouse clicks.
    
       When a mouse click (default: left click) occurs within the specified
       rectangular region the given function will be called."""
    
    def __init__(self, kind="click", x1=None, y1=None, x2=None, y2=None, 
                 func=None, give_pos=False, **options):
        """kind     - must be one of these strings: "click", "drag"
           x1       - 1st x-coordinate of rectangular region
           y1       - 1st y-coordinate of rectangular region
           x2       - 2nd x-coordinate of rectangular region
           y2       - 2nd y-coordinate of rectangular region
           func     - callback function of no arguments
           give_pos - a bool determining whether the mouse position should be
                      given to the function 'func'
          
          Requirements of callback func:
          - for hotspot("click", x1, y1, x2, y2, func)
            func must except no arguments
            
          - for hotspot("click", x1, y1, x2, y2, func, give_pos=True)
            func must except two arguments (x, y) which are the location of
            the mouse click in local world coordinates
        
          - for hotspot("drag", x1, y1, x2, y2, func)
            func must except three arguments (state, x, y) which are the 
            location of the mouse click in local world coordinates and the
            drag event ("drag", "drag_start", "drag_stop")
        """
        
        if kind == "drag":
            def func2(event, x, y):
                func(_int2hotspot_event[event], x, y)
            Element.__init__(self, _HOTSPOT_CONSTRUCT, 
                             (kind, x1, y1, x2, y2, func2, give_pos), options)
        else:
            Element.__init__(self, _HOTSPOT_CONSTRUCT, 
                             (kind, x1, y1, x2, y2, func, give_pos), options)
            


def hotspot_custom(kind, x1, y1, x2, y2, detect, func, give_pos=False):
    """
    Designates a region (of arbitrary shape) of the screen to react to 
    mouse clicks.
    
    When a mouse click (default: left click) occurs within the specified
    rectangular region (x1,y1)-(x2,y2), the function  detect(mouse_x, mouse_y)
    will be called.  If it returns True the given function 'func' will be called.
    The coordinates (x1, y1, x2, y2) are given to efficiently filter mouse 
    clicks such that detect() is only called for the those clicks that are most
    likely to collide with the hotspot.
    """
    
    if kind == "drag":
        def detect2(event, px, py):
            if detect(px, py):
                func(event, px, py)
    else:
        def detect2(px, py):
            if detect(px, py):
                if give_pos:
                    func(px, py)
                else:
                    func()
        
    return hotspot(kind, x1, y1, x2, y2, detect2, give_pos=True)


def hotspot_circle(kind, x, y, radius, func, give_pos=False):
    """
    Designates a circular region of the screen to react to 
    mouse clicks.
    """
    def detect(px, py):
        return math.sqrt((x-px)**2 + (y-py)**2) < radius
        
    return hotspot_custom(kind, x-radius, y-radius, x+radius, y+radius, 
                          detect, func, give_pos=give_pos)


def hotspot_polygon(kind, pts, func, give_pos=False):
    """
    Designates a convex polygonal region of the screen to react to 
    mouse clicks.
    
    pts - a list of points that define the polygon
          ex: [x1, y1, x2, y2, x3, y3, x4, y4]
    """
    x = []
    y = []
    pts2 = []
    for i in xrange(0, len(pts), 2):
        x.append(pts[i])
        y.append(pts[i+1])
        pts2.append((pts[i], pts[i+1]))
    
    def detect(px, py):
        return vector.in_polygon2(pts2, (px, py))
    
    return hotspot_custom(kind, min(x), min(y), max(x), max(y), 
                          detect, func, give_pos=give_pos)


def hotspot_region(kind, region, func, give_pos=False):
    """
    Designates a region (summon.Region) of the screen to react to mouse clicks
    """
    
    return hotspot_custom(kind, region.x1, region.y1, 
                          region.x2, region.y2, region.detect, func, 
                          give_pos=give_pos)

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
    def __init__(self, txt="", x1=None, y1=None, x2=None, y2=None, *justified,
                       **options):
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
                               _tuple(txt, x1, y1, x2, y2, *justified),  
                               options)



class text_scale (text):
    """A vector graphics text element"""
    def __init__(self, txt="", x1=None, y1=None, x2=None, y2=None, *justified,
                 **options):
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
                                 _tuple(txt, x1, y1, x2, y2, *justified), 
                                 options)


class text_clip (custom_group, text):
    """A vector graphics text element that has a minimum and maximum height"""
    def __init__(self, txt="", x1=None, y1=None, x2=None, y2=None,
                       minheight=4, maxheight=20, *justified,
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
        
        # ensure points are ordered correctly
        if x1 > x2:
            x1, x2 = x2, x1
        if y1 > y2:
            y1, y2 = y2, y1
        
        # save parameters for get() method
        self.txt = txt
        self.coords = (x1, y1, x2, y2)
        self.height_clamp = (minheight, maxheight)
        self.justified = justified
        
        # calculate text dimensions
        textw = float(summon_core.get_text_width(0, txt))
        texth = float(summon_core.get_text_height(0))
        textr = textw / texth
        minwidth = minheight * textr
        maxwidth = maxheight * textr
        
        # calculate box dimensions
        boxw = float(abs(x1-x2))
        boxh = float(abs(y1-y2))        
        boxr = boxw / boxh
        
        # determine which dimension is tight (adjust textw/h accordingly)
        if textr > boxr:
            textw = boxw 
            texth = boxw / textr
        else:
            textw = textr * boxh 
            texth = boxh
        
        # justify text within box (determine zoom_clamp origin)
        # default origin
        ox = x1
        oy = (y1 + y2) / 2.0
        if "left" in justified:
            ox = x1
        if "right" in justified:
            ox = x2
        if "center" in justified:
            ox = (x1 + x2) / 2.0
        if "top" in justified:
            oy = y2
        if "bottom" in justified:
            oy = y1
        if "middle" in justified:
            oy = (y1 + y2) / 2.0        
        
        # init drawing elements
        custom_group.__init__(self, 
            zoom_clamp(
                text_scale(txt, x1, y1, x2, y2, *justified),
                #lines(x1, y1, x2, y2),

                miny=minheight / texth, maxy=maxheight / texth,
                minx=minwidth / textw, maxx=maxwidth / textw,
                link=True, link_type="smaller", 
                origin=(ox, oy), axis=(ox+1.0, oy),
                clip=True,
                prezoom=(boxw/textw, boxh/texth)))
    
    
    def get(self):
        coords = self.coords
        return _tuple(_TEXT_CLIP_CONSTRUCT, self.txt, coords[0], coords[1], coords[2],
                      coords[3], self.height_clamp[0], self.height_clamp[1],
                      *self.justified)


'''
class text_clip (text):
    """A vector graphics text element that has a minimum and maximum height"""
    def __init__(self, txt="", x1=None, y1=None, x2=None, y2=None,
                       minheight=4, maxheight=20, *justified,
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
                  _tuple(txt, x1, y1, x2, y2, minheight, maxheight, *justified), 
                  options)
'''


#=============================================================================
# transforms

class Transform (Element):
    """Base class for all transformation elements"""
    def __init__(self, *args, **options):
        Element.__init__(self, _TRANSFORM_CONSTRUCT, args, options)

class translate (Transform):
    """Translates the containing elements"""
    def __init__(self, x=None, y=None, *elements, **options):
        """x         - units along the x-axis to translate
           y         - units along the y-axis to translate
           *elements - one or more elements to translate
        """
        Element.__init__(self, _TRANSLATE_CONSTRUCT, 
                         _tuple(x, y, *elements), options)

class rotate (Transform):
    """Rotates the containing elements"""
    def __init__(self, angle=None, *elements, **options):
        """angle     - angle in degrees (-360, 360) to rotate (clock-wise)
           *elements - one or more elements to rotate
        """
        Element.__init__(self, _ROTATE_CONSTRUCT, 
                         _tuple(angle, *elements), options)

class scale (Transform):
    """Scales the containing elements"""
    def __init__(self, scalex=None, scaley=None, *elements, **options):
        """scalex    - factor of scaling along x-axis
           scaley    - factor of scaling along y-axis
           *elements - one or more elements to scale
        """
        Element.__init__(self, _SCALE_CONSTRUCT, 
                         _tuple(scalex, scaley, *elements), options)

class flip (Transform):
    """Flips the containing elements over a line (0,0)-(x,y)"""
    def __init__(self, x=None, y=None, *elements, **options):
        """x         - x-coordinate of line
           y         - y-coordinate of line
           *elements - one or more elements to flips
        """
        Element.__init__(self, _FLIP_CONSTRUCT, 
                         _tuple(x, y, *elements), options)




class zoom_clamp (Transform):
    """Restricts the zoom-level of its contents
    
       See shapes.message_bubble() for an example
    """
    
    def __init__(self, 
                 *elements, **options):
        """*elements  -- elements to apply clamp to
           minx       -- minimum x-zoom level (default: 0.0)
           miny       -- minimum y-zoom level (default: 0.0)
           maxx       -- maximum x-zoom level (default: inf)
           maxy       -- maximum y-zoom level (default: inf)
           clip       -- a bool whether elements are cliped when too small
                         (default: False)
           link       -- a bool whether x- and y-axis of elements
                         should always zoom together (default: False)
           link_type  -- a string indicating the rule for linking
                         "smaller" use the smaller of the x and y zooms
                         "larger" use the larger of the x and zooms
                         (default: "larger")
           origin     -- a tuple (x, y) that defines the point at which the
                         zoom_clamp is stationary with its parent .
                         (default: (0,0))
           axis       -- a tuple (ax, ay) that together with origin define
                         a line that is stationary with is parent
                         (default: (origin[0]+1, origin[1]))
           prezoom    -- apply an extra zoom (zx, zy) to the contents before 
                         clamping
        """
        
        minx = options.get("minx", 0.0)
        miny = options.get("miny", 0.0)
        maxx = options.get("maxx", _INF)
        maxy = options.get("maxy", _INF)
        clip = options.get("clip", False)
        link = options.get("link", False)
        link_type = options.get("link_type", "larger")
        origin = options.get("origin", (0.0, 0.0))
        axis = options.get("axis", origin)
        prezoom = options.get("prezoom", (1.0, 1.0))
        
        assert "xmax" not in options and \
               "ymax" not in options and \
               "xmin" not in options and \
               "ymin" not in options, \
               Exception("Arguments are misspelled (i.e. should be maxy not ymax)")
        
        assert len(origin) == 2, Exception("origin must be a tuple of 2 floats")
        assert len(axis) == 2, Exception("axis must be a tuple of 2 floats")
        
        if link:
            low = max(minx, miny)
            high = min(maxx, maxy)
            if abs(low - high) / (low + high) < 1e-10: low = high
            
            assert low <= high, Exception("x (%f) and y (%f) clamp range must intersect if linked" % (low, high))
            minx = miny = low
            maxx = maxy = high
        else:
            if abs(minx - maxx) / (minx - maxx) < 1e-10: minx = maxx
            if abs(miny - maxy) / (miny - maxy) < 1e-10: miny = maxy
            assert minx <= maxx, Exception("minx cannot be greater than maxx")
            assert miny <= maxy, Exception("miny cannot be greater than maxy")
        
        if link_type == "smaller":
            link_type = False
        elif link_type == "larger":
            link_type = True
        else:
            raise Exception("unknown link_type '%s'" % link_type)
        
        Element.__init__(self, _ZOOM_CLAMP_CONSTRUCT, 
                         _tuple(minx, miny, maxx, maxy, clip, link, link_type,
                                origin[0], origin[1], axis[0], axis[1],
                                prezoom[0], prezoom[1],
                                *elements), 
                         options)
    
    # TODO: add wrapper for get_contents to convert link_type back to a str

 
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
    _FLIP_CONSTRUCT:           flip,
    _ZOOM_CLAMP_CONSTRUCT:     zoom_clamp
}


def _make_ref(elementid, ptr):
    if elementid == _CUSTOM_GROUP_CONSTRUCT:
        return summon_core.get_element_contents(ptr)
    else:
        return _element_table[elementid](ref=ptr)


def _tuple(*args):
    return args


