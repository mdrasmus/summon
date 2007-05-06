"""
    SUMMON
    visualization prototyping and scripting

    Matt Rasmussen

    SUMMON is a python extension module that provides rapid prototyping of 2D
    visualizations. By heavily relying on the python scripting language, SUMMON
    allows the user to rapidly prototype a custom visualization for their data,
    without the overhead of a designing a graphical user interface or
    recompiling native code. By simplifying the task of designing a
    visualization, users can spend more time on understanding their data.


"""

import os, sys
from summon.core import *


state = get_summon_state()


def load_config():
    """Load a summon config file"""
    
    # look for config in HOME directory
    if "HOME" in os.environ:
        config_file = os.path.join(os.environ["HOME"], ".summon_config")
    else:
        config_file = ""        
    if not config_file or not os.path.exists(config_file):
        # loof for config in python paths
        try:
            import summon_config
            reload(summon_config)
        except Exception, e:
            print "could not import summon_config"
            print e
    
    else:
        execfile(config_file)



#=============================================================================
# Dynamic updating interface
#

"""
SUMMON provides a timer that calls a python function of your choice at regular
intervals.  The following functions provide an interface to the SUMMON timer.

"""

def add_update_func(func, win):
    """
    adds a function of no arguments to the list of functions called by SUMMON
    at regular intervals
    """
    state.updateFuncs.append((func, win))

def remove_update_func(func):
    """
    removes a function from the list of SUMMON timer functions
    """
    state.updateFuncs = filter(lambda x: x[0] != func, state.updateFuncs)

def is_update_func(func):
    """returns True if 'func' is currently registered as a SUMMON timer function"""
    return func in (x[0] for x in state.updateFuncs)

def get_update_funcs():
    """returns all functions registered as SUMMON timer functions"""
    return state.updateFuncs

def set_update_interval(secs):
    """sets the timer interval between calls the to the timer function"""
    state.updateInterval = secs


def begin_updating():
    """starts the SUMMON timer"""
    
    windows = set(get_windows())
    dels = set()
    
    for i, (func, win) in enumerate(state.updateFuncs):
        if win.winid in windows:
            win.activate()
            func()
        else:
            dels.add(func)
    
    # remove closed windows
    if len(dels) > 0:
        state.updateFuncs = filter(lambda x: x[0] not in dels, state.updateFuncs)
    
    timer_call(state.updateInterval, begin_updating)
    
def stop_updating():
    """stops the SUMMON timer"""
    timer_call(0, lambda :None)



#=============================================================================
# Visdraw window objects
#

class Window (object):
    """The SUMMON Window"""

    def __init__(self, name="SUMMON", worldid=None, screenid=None):
        # create new window
        self.winid = new_window()
        assert self.winid != None
        state.add_window(self)

        if worldid != None:
            self.world = state.get_model(worldid)
        else:
            self.world = Model()

        if screenid != None:
            self.screen = state.get_model(screenid)
        else:
            self.screen = Model()

        summon_core.assign_model(self.winid, "world", self.world.id)
        summon_core.assign_model(self.winid, "screen", self.screen.id)
        
        
        self.antialias = True
        self.crosshair = False
        self.crosshair_color = None
        
        # load default configuration
        self.activate()
        load_config()

        self.set_name(name)
    
    
    def activate(self):
        """make this window the current window (deprecated)"""
        summon_core.set_window(self.winid)
        state.current_window = self
    
    
    # view
    def close(self):
        #self.onClose()
        state.remove_window(self)
        return summon_core.close_window(self.winid)
    close.__doc__ = summon_core.close_window.__doc__.split("\n")[1]
    
    #def onClose(self):
    #    """Subclass to detect window close events"""
    #    pass
    # TODO: need to also capture events when window is closed by X button
    #
    
    def set_name(self, name):
        self.name = name
        return summon_core.set_window_name(self.winid, name)
    set_name.__doc__ = summon_core.set_window_name.__doc__.split("\n")[1]
    
    def get_name(self, name):
        """returns the name of a window"""
        return self.name
    
    def set_size(self, width, height):
        return summon_core.set_window_size(self.winid, width, height)
    set_size.__doc__ = summon_core.set_window_size.__doc__.split("\n")[1]
    
    def get_size(self):
        return summon_core.get_window_size(self.winid)
    get_size.__doc__ = summon_core.get_window_size.__doc__.split("\n")[1]

    def set_position(self, x, y):
        return summon_core.set_window_position(self.winid, x, y)
    set_position.__doc__ = summon_core.set_window_position.__doc__.split("\n")[1]
    
    def get_position(self):
        return summon_core.get_window_position(self.winid)
    get_position.__doc__ = summon_core.get_window_position.__doc__.split("\n")[1]
    
    def focus(self, x, y):
        return summon_core.focus(self.winid, int(x), int(y))
    focus.__doc__ = summon_core.focus.__doc__.split("\n")[1]
    
    def zoom(self, x, y):
        return summon_core.zoom(self.winid, x, y)
    zoom.__doc__ = summon_core.zoom.__doc__.split("\n")[1]

    def zoomx(self, x):
        return summon_core.zoomx(self.winid, x)
    zoomx.__doc__ = summon_core.zoomx.__doc__.split("\n")[1]
    
    def zoomy(self, y):
        return summon_core.zoomy(self.winid, y)
    zoomy.__doc__ = summon_core.zoomy.__doc__.split("\n")[1]
    
    def zoom_camera(self, factor, factor2=None):
        """returns function that will zoom a window by a fixed factor.
           handy for use with set_binding()
           
           if two zoom factors are supplied, the x and y coordinates will be
           zoomed separately.
        """
        
        if factor2 == None:
            factor2 = factor

        def func():
            w, h = self.get_size()
            self.focus(w/2.0, h/2.0)
            self.zoom(factor, factor2)
        return func
    
    def set_trans(self, x, y):
        return summon_core.set_trans(self.winid, x, y)
    set_trans.__doc__ = summon_core.set_trans.__doc__.split("\n")[1]
    
    def get_trans(self):
        return summon_core.get_trans(self.winid)
    get_trans.__doc__ = summon_core.get_trans.__doc__.split("\n")[1]
    
    def set_zoom(self, x, y):
        return summon_core.set_zoom(self.winid, x, y)
    set_zoom.__doc__ = summon_core.set_zoom.__doc__.split("\n")[1]

    def get_zoom(self):
        return summon_core.get_zoom(self.winid)
    get_zoom.__doc__ = summon_core.get_zoom.__doc__.split("\n")[1]
    
    def get_focus(self):
        return summon_core.get_focus(self.winid)
    get_focus.__doc__ = summon_core.get_focus.__doc__.split("\n")[1]
    
    def set_focus(self, x, y):
        return summon_core.set_focus(self.winid, x, y)
    set_focus.__doc__ = summon_core.set_focus.__doc__.split("\n")[1]
    
    def trans(self, x, y):
        return summon_core.trans(self.winid, x, y)
    trans.__doc__ = summon_core.trans.__doc__.split("\n")[1]
    
    def trans_camera(self, x, y):
        """Return a function of no arguments that will translate the camera"""
        return lambda: self.trans(x, y)
    
    def home(self):
        return summon_core.home(self.winid)
    home.__doc__ = summon_core.home.__doc__.split("\n")[1]
    
    def set_bgcolor(self, r, g, b):
        return summon_core.set_bgcolor(self.winid, r, g, b)
    set_bgcolor.__doc__ = summon_core.set_bgcolor.__doc__.split("\n")[1]
    
    def get_bgcolor(self):
        return summon_core.get_bgcolor(self.winid)
    get_bgcolor.__doc__ = summon_core.get_bgcolor.__doc__.split("\n")[1]
    
    def set_visible(self, x1, y1, x2, y2):
        return summon_core.set_visible(self.winid, x1, y1, x2, y2)
    set_visible.__doc__ = summon_core.set_visible.__doc__.split("\n")[1]
    
    def get_visible(self):
        return summon_core.get_visible(self.winid)
    get_visible.__doc__ = summon_core.get_visible.__doc__.split("\n")[1]
    
    def set_antialias(self, enabled):
        self.antialias = enabled
        return summon_core.set_antialias(self.winid, enabled)
    set_antialias.__doc__ = summon_core.set_antialias.__doc__.split("\n")[1]
    
    def toggle_aliasing(self):
        self.set_antialias(not self.antialias)

    def toggle_crosshair(self):
        self.show_crosshair(not self.crosshair)

        if self.crosshair_color == None:
            col = self.get_bgcolor()
            self.set_crosshair_color(1-col[0], 1-col[1], 1-col[2], 1)
    
    def set_crosshair_color(self, r, g, b, a=1):
        self.crosshair_color = (r, g, b, a)
        return summon_core.set_crosshair_color(self.winid, r, g, b, a)
    set_crosshair_color.__doc__ = summon_core.set_crosshair_color.__doc__.split("\n")[1]

    def show_crosshair(self, enabled):
        self.crosshair = enabled
        return summon_core.show_crosshair(self.winid, enabled)
    show_crosshair.__doc__ = summon_core.show_crosshair.__doc__.split("\n")[1]
    
    
    
    # controller
    def add_binding(self, input_obj, func):
        """add a function to the list of functions bounded to an input"""
        return summon_core.set_binding(self.winid, input_obj, func)
        
    def clear_binding(self, input_obj):
        return summon_core.clear_binding(self.winid, input_obj)
    clear_binding.__doc__ = summon_core.clear_binding.__doc__.split("\n")[1]

    def set_binding(self, input_obj, func):
        summon_core.clear_binding(self.winid, input_obj)
        return summon_core.set_binding(self.winid, input_obj, func)
    set_binding.__doc__ = summon_core.set_binding.__doc__.split("\n")[1]        
    reset_binding = set_binding

    
    def clear_all_bindings(self):
        return summon_core.clear_all_bindings(self.winid)
    clear_all_bindings.__doc__ = summon_core.clear_all_bindings.__doc__.split("\n")[1]

    def get_mouse_pos(self, coord):
        """gets the current mouse position within the window
        
           coords -- either 'world', 'screen', or 'window'.
                     mouse position will be returned as a coordinate in the
                     specified coordinate system
        """          
           
        return summon_core.get_mouse_pos(self.winid, coord)
    
    
        
    # model manipulation (forward to world)
    def clear_groups(self):
        return self.world.clear_groups()
    clear_groups.__doc__ = summon_core.clear_groups.__doc__.split("\n")[1]
    
    def add_group(self, aGroup):
        return self.world.add_group(aGroup)
    add_group.__doc__ = summon_core.add_group.__doc__.split("\n")[1]
    
    def insert_group(self, groupid, aGroup):
        """inserts a drawing group 'aGroup' as a child to an existing group
           with id 'groupid'."""
        return self.world.insert_group(groupid, aGroup)
    
    def remove_group(self, *groupids):
        return self.world.remove_group(*groupids)
    remove_group.__doc__ = summon_core.remove_group.__doc__.split("\n")[1]
    
    def replace_group(self, groupid, aGroup):
        """replaces a current drawing group with id 'groupid' with a new 
           group 'aGroup'"""
        return self.world.replace_group(groupid, aGroup)
    replace_group.__doc__ = summon_core.replace_group.__doc__.split("\n")[1]
    
    def show_group(self, groupid, visible):
        return self.world.show_group(groupid, visible)
    show_group.__doc__ = summon_core.show_group.__doc__.split("\n")[1]
    
    def get_group(self, groupid):
        return self.world.get_group(groupid)
    get_group.__doc__ = summon_core.get_group.__doc__.split("\n")[1]
    
    def get_root_id(self):
        return self.world.get_root_id()
    get_root_id.__doc__ = summon_core.get_root_id.__doc__.split("\n")[1]
    
    
    # misc
    def duplicate(self):
        """returns a new window with same model and properties as this window"""
    
        # get current window, model, and visible
        coords = self.get_visible()
        size = self.get_size()
        bg = self.get_bgcolor()

        zoomx = (coords[2] - coords[0]) / size[0]
        zoomy = (coords[3] - coords[1]) / size[1]

        # create new window with same model and coords    
        win = Window(worldid=self.world.id)
        win.set_size(* size)
        win.set_bgcolor(* bg)
        
        # make the view the same
        win.set_visible(*coords)
        coords = win.get_visible()
        zoomx2 = (coords[2] - coords[0]) / size[0]
        zoomy2 = (coords[3] - coords[1]) / size[1]
        win.focus(size[0] / 2, size[1] / 2)
        win.zoom(zoomx2 / zoomx, zoomy2 / zoomy)
        
        
        # load up the configuration
        win.activate()
        load_config()
        
        return win                




class Model (object):
    """A Model contains graphics for display.
    
       Each window has a 'world' and 'screen' model.
    """

    def __init__(self, modelid = None):
        if modelid == None:
            # create new window
            self.id = summon_core.new_model()
            assert self.id != None
            state.add_model(self)
    
    
    def clear_groups(self):
        return summon_core.clear_groups(self.id)
    clear_groups.__doc__ = summon_core.clear_groups.__doc__.split("\n")[1]
    
    def add_group(self, aGroup):
        return summon_core.add_group(self.id, aGroup)
    add_group.__doc__ = summon_core.add_group.__doc__.split("\n")[1]
    
    def insert_group(self, groupid, aGroup):
        return summon_core.insert_group(self.id, groupid, aGroup)
    insert_group.__doc__ = summon_core.insert_group.__doc__.split("\n")[1]
    
    def remove_group(self, *groupids):
        return summon_core.remove_group(self.id, *groupids)
    remove_group.__doc__ = summon_core.remove_group.__doc__.split("\n")[1]
    
    def replace_group(self, groupid, aGroup):
        return summon_core.replace_group(self.id, groupid, aGroup)
    replace_group.__doc__ = summon_core.replace_group.__doc__.split("\n")[1]

    def show_group(self, groupid, visible):
        return summon_core.show_group(self.id, groupid, visible)
    show_group.__doc__ = summon_core.show_group.__doc__.split("\n")[1]
    
    def get_group(self, groupid):
        return summon_core.get_group(self.id, groupid)
    get_group.__doc__ = summon_core.get_group.__doc__.split("\n")[1]

    def get_root_id(self):
        return summon_core.get_root_id(self.id)
    get_root_id.__doc__ = summon_core.get_root_id.__doc__.split("\n")[1]



class VisObject (object):
    """Base class of dynamic visualization objects"""
    
    def __init__(self):
        self.win = None
    
    def __del__(self):
        if state != None:
            self.enableUpdating(False)
    
    def update(self):
        """this function is called from the SUMMER timer"""
        pass
    
    def show(self):
        pass
    
    def enableUpdating(self, visible=True):
        """add/remove this object's update function to/from the SUMMON timer"""
        if visible:    
            if not is_update_func(self.update):
                assert self.win != None, "must set window"
                add_update_func(self.update, self.win)
        else:
            if is_update_func(self.update):
                remove_update_func(self.update)
    
    # temporary backwards compatibility
    setVisible = enableUpdating
    
    
    def get_window(self):
        return self.win



#=============================================================================
# functions for iterating and inspecting graphical elements
#

def is_graphic(elm):
    return is_points(elm) or \
           is_lines(elm) or \
           is_line_strip(elm) or \
           is_triangles(elm) or \
           is_triangle_strip(elm) or \
           is_triangle_fan(elm) or \
           is_quads(elm) or \
           is_quad_strip(elm) or \
           is_polygon(elm)

def is_text_elm(elm):
    return is_text(elm) or \
           is_text_scale(elm) or \
           is_text_clip(elm)

def is_primitive(elm):
    return is_vertices(elm) or is_color(elm)


def is_transform(elm):
    return is_translate(elm) or \
           is_scale(elm) or \
           is_rotate(elm) or \
           is_flip(elm)

def graphic_contents(elm):
    return elm[1:]

def element_contents(elm):
    if is_group(elm):
        return group_contents(elm)
    elif is_translate(elm):
        #return translate_content(elm)[2:]
        return elm[3:]
    elif is_scale(elm):
        #return scale_contents(elm)[2:]
        return elm[3:]
    elif is_flip(elm):
        return flip_contents(elm)[2:]
        return elm[3:]
    elif is_rotate(elm):
        return rotate_contents(elm)[1:]
        return elm[2:]
    else:
        return ()


def visitElements(elm, beginFunc, endFunc):
    beginFunc(elm)
    for elm in element_contents(elm):
        visitElements(elm, beginFunc, endFunc)
    endFunc(elm)


def visitGraphics(elm, func):
    closure = {
        'verts': [],
        'color': [1, 1, 1, 1]
    }

    def processGraphic(elm, nverts, nkeep):
        if len(closure['verts']) == nverts:
            func(elm, closure['verts'], closure['color'])
            if nkeep == 0:
                closure['verts'] = []
            else:
                closure['verts'] = closure['verts'][-nkeep:]

    def beginElement(elm):
        if is_graphic(elm):
            closure["verts"] = []
            for prim in graphic_contents(elm):
                if is_color(prim):
                    rgb = list(color_contents(prim))
                    if len(rgb) == 3:
                        rgb.append(1)
                    closure['color'] = rgb

                elif is_vertices(prim):
                    coords = vertices_contents(prim)
                    for i in range(0, len(coords), 2):
                         # push a new vertex                    
                         closure['verts'].append(coords[i:i+2])

                         if is_points(elm):           processGraphic(elm, 1, 0)
                         elif is_lines(elm):          processGraphic(elm, 2, 0)
                         elif is_line_strip(elm):     processGraphic(elm, 2, 1)
                         elif is_triangles(elm):      processGraphic(elm, 3, 0)
                         elif is_triangle_strip(elm): processGraphic(elm, 3, 2)
                         elif is_quads(elm):          processGraphic(elm, 4, 0)
                         elif is_quad_strip(elm):     processGraphic(elm, 4, 2)
            if is_polygon(elm):
                processGraphic(elm, len(closure['verts']), 0)
        elif is_text_elm(elm):
            verts = text_contents(elm)[1:5]
            func(elm, [verts[:2], verts[2:]], closure['color'])
        else:
            func(elm, [], closure['color'])

    def endElement(elm):
        return None

    visitElements(elm, beginElement, endElement)



#=============================================================================
# additonal modules imported here
#

from summon.colors import *
