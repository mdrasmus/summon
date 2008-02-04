"""
    SUMMON
    visualization prototyping and scripting

    Copyright Matt Rasmussen 2005-2008

    SUMMON is a python extension module that provides rapid prototyping of 2D
    visualizations. By heavily relying on the python scripting language, SUMMON
    allows the user to rapidly prototype a custom visualization for their data,
    without the overhead of a designing a graphical user interface or
    recompiling native code. By simplifying the task of designing a
    visualization, users can spend more time on understanding their data.


"""

import os, sys
from time import time as get_time

from summon.core import *
from summon import util, select, core
import summon.svg

import summon_config


VERSION = "1.8.5"

VERSION_INFO = """\
-----------------------------------------------------------------------------
                                  SUMMON %s
                      visualization prototyping and scripting
                                Matt Rasmussen
                            (http://mit.edu/rasmus)
                              Copyright 2005-2007
-----------------------------------------------------------------------------
""" % VERSION



def version():
    print VERSION_INFO




#=============================================================================S
# python state of SUMMON
#
class SummonState (object):
    """SUMMON State
    
       This class is instantiated as a singleton when the summon module is 
       loaded.  It maintains the list of open windows and models as well as
       the global summon timer object.  This class and singleton object should
       not be used directly by users.
    """

    def __init__(self):
        self.current_window = None
        self.windows = {}
        self.models = {}
        self.timer = TimerDispatch()
        
    
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
    
    def _on_window_close(self, winid):
        self.windows[winid]._on_close()


#=============================================================================
# timer interface
#

class Timer:
    def __init__(self, dispatch, func, interval=None, repeat=True, window=None):
        self._dispatch = dispatch
        self._func = func
        self._win = window
        self._interval = interval
        self._repeat = repeat
        self._deadline = get_time() + interval
        self._enabled = True

    def start(self):
        if not self.enabled:
            self._enabled = True
            self._dispatch.add_timer(self)

    def stop(self):
        if self._enabled:
            self._enabled = False
            self._dispatch.remove_timer(self)
        
    def set_interval(self, interval):
        self._interval = interval
    
    def get_interval(self):
        return self._interval
    
    def reset(self, now):
        if self._repeat:
            self._deadline = now + self._interval
            return True
        else:
            self._deadline = util.INF
            return False
    
    def get_delay(self, now):
        return self._deadline - now
    
    def is_expired(self, now):
        return self._deadline < now
    
    def set_repeating(self, repeat):
        self._repeat = repeat
    
    def is_repeating(self):
        return self._repeat
    
    def call(self):
        self._func()
    
    def is_win_open(self):
        return self._win == None or self._win.is_open()


class TimerDispatch:
    """
    SUMMON provides a timer that calls a python function of your choice at
    regular intervals.  The following functions provide an interface to the
    SUMMON timer.

    """

    def __init__(self):
        self.timers = []
        self.default_interval = .5
        self.timestep = 0
        
    
    def add_timer(self, func, interval=None, repeat=True, window=None):
        """
        adds a function of no arguments to the list of functions called by SUMMON
        at regular intervals
        """
        if interval == None:
            interval = self.default_interval
        timer = Timer(self, func, interval, repeat, window)
        self.timers.append(timer)
        self.start()
        return timer
        

    def remove_timer(self, timer):
        """
        removes a timer from the list of SUMMON timer functions
        """
        self.timers.remove(timer)
    
    
    def set_default_interval(self, secs):
        """sets the default timer interval"""
        self.default_interval = secs

    
    def start(self):
        """starts the SUMMON timer"""
        
        dels = set()
        
        now = get_time()
        
        # call each timer that has past
        mindelay = util.INF
        for timer in self.timers:
            if timer.is_win_open():
                # process timer if window is ok (open)
                
                if timer.is_expired(now):
                    # call user function
                    try:
                        timer.call()
                    except Exception, e:
                        print "exception in SUMMON timer:", e
                    
                    # reset timer
                    if not timer.reset(now):
                        # delete timer
                        dels.add(timer)
                
                mindelay = min(mindelay, timer.get_delay(now))
            else:
                # remove functions of closed windows
                dels.add(timer)

        # remove closed windows
        for d in dels:
            self.timers.remove(d)
        
        # setup next call
        if mindelay < util.INF:
            summon_core.timer_call(max(0, mindelay), self.start)
    
    
    def stop(self):
        """stops the SUMMON timer"""
        summon_core.timer_call(0, lambda :None)


#=============================================================================
# module interface 

# global singleton
_state = SummonState()
        

def get_windows():
    """returns the currently open windows"""
    return _state.windows.itervalues()

def get_window_by_name(name):
    """returns a window by its name"""
    for window in get_windows():
        if name == window.get_name():
            return window
    return None
        

def get_summon_state():
    """returns the summon state singleton"""
    return _state

# install summon window close callback for communication between C++ and python
summon_core.set_window_close_callback(_state._on_window_close)


# window decoration
_window_decoration = summon_core.get_window_decoration()
def get_window_decoration():
    return _window_decoration

def set_window_decoration(xoffset, yoffset):
    global _window_decoration
    _window_decoration = (xoffset, yoffset)


def add_timer(func, interval=None, repeat=True, window=None):
    """Returns a new SUMMON timer that will call"""
    return _state.timer.add_timer(func, interval, repeat, window=window)



#=============================================================================
# Window and Model
#

class Window (object):
    """The SUMMON Window
    
       Each window opened by SUMMON is an instantiation of this class. This
       class provides the interface to manipulate the window (title, size,
       position, etc.) as well as to manipulate the graphical elements it 
       displays (add_group, remove_group, etc.).  

       Each window is associated with two models (see Model class), called the
       'world' and 'screen' which are accessible as 'Window.world' and
       'Window.screen'.  The units of the world model's coordinate system  can
       be whatever the user wants, provided that the x- and y- axis increase to 
       the right and top, respectively.   The size and orientation of the world 
       model is dependent on scrolling and zooming. The world model is the
       default model that is  used when drawing (e.g. functions such as
       Window.add_group forward their arguments to Window.world.add_group).

       In contrast, the screen model is always drawn with its origin at the 
       lower left corner of the window and with units in terms of pixels. The
       screen model is always drawn on top of the world model.  The screen model
       is often used to draw menus, toolbars, or other things that should always
       be in view, and should not be affected by scrolling and zooming.
    """

    def __init__(self, name="SUMMON", 
                 position=None, size=(400, 400),
                 world=None, screen=None,
                 winconfig=None):
        """name       -- title on window
           position   -- the initial position (x,y) of the window on the desktop
           size       -- the initial size (width, height) of the window
           world      -- the world model
           screen     -- the screen model
           winconfig -- a function that will be called with the window
                        as its argument.  Any duplicates of the window will
                        also use the same configuration function.
        """
                
        # create new window
        if position == None:
            position = (-1, -1)
        
        self.winid = summon_core.new_window(name, size[0], size[1],
                                            position[0], position[1])
        assert self.winid != None
        _state.add_window(self)
        
        
        # setup world model
        if world != None:
            self.world = world
        else:
            self.world = Model()
        summon_core.assign_model(self.winid, "world", self.world.id)
        
        # setup screen model
        if screen != None:
            self.screen = screen
        else:
            self.screen = Model()
        summon_core.assign_model(self.winid, "screen", self.screen.id)
        
        # listeners
        self.viewChangeListeners = set()
        self.focusChangeListeners = set()
        self.closeListeners = set()
        self.resizeListeners = set()
        self.moveListeners = set()
        self.viewLock = False
        self.focusLock = False
        summon_core.set_window_on_resize(self.winid, self._on_resize)
        summon_core.set_window_on_move(self.winid, self._on_move)     
        
        # configure window
        self.name = name
        self.antialias = False
        self.crosshair = False
        self.crosshair_color = None
        self.opened = True
        
        # menu
        self.menu = None
        self.menuButton = 1  # middle mouse button

        # selection
        self.select = select.Select(self, lambda x,y:None)        
        
        # load default configuration
        self.winconfig = winconfig
        self.load_config(winconfig)
        

    
    def set_world_model(self, model):
        """sets a model to be the world model"""
        summon_core.assign_model(self.winid, "world", model.id)
        self.world = model
    
     
    def set_screen_model(self, model):
        """sets a model to be the world model"""
        summon_core.assign_model(self.winid, "screen", model.id)
        self.screen = model
           
    
    def load_config(self, winconfig=None):
        """loads a summon config file for a window"""
        _state.current_window = self
        
        if winconfig != None:
            # if configuration function given, then use it
            winconfig(self)
            return
        
        if "HOME" in os.environ:
            # look for config in HOME directory
            
            config_file = os.path.join(os.environ["HOME"], ".summon_config")
            if os.path.exists(config_file):
                # execute config file
                glob = {}
                local = {}
                execfile(config_file, glob, local)
                
                if "config_window" not in local:
                    raise "Must define function 'config_window(win)' in '%s'" % config_file
                
                # run configuration function
                local["config_window"](self)
                return
                
        # try to load config from python path
        try:
            summon_config.config_window(self)
        except Exception, e:
            print "Warning: could not import summon_config"
            print e

    #===========================================================
    # window properties
    
    def is_open(self):
        """returns whether underling SUMMON window is open"""
        
        return self.opened and self.winid in _state.windows
    
    def get_decoration(self):
        """returns the window decoration size (width, height)
           
           Decoration size refers to the 'width' of the side frame and the
           'height' of the title bar.
        """
        return _window_decoration    
    

    def close(self):
        """close the window"""
        
        if not self.opened:
            return
        
        # immediately record locally that window is closed
        self.opened = False
        try:
            summon_core.close_window(self.winid)
        except:
            pass
        
        # self._on_close() will get called back through the C++ module
        # letting us know the window is truely closed
        return
    
    def _on_close(self):
        """a callback for window close events"""
        
        self.opened = False
        
        # let the global summon state know, that this window is closed
        _state.remove_window(self)
        
        # let all listeners know this window is closed
        for listener in list(self.closeListeners):
            listener()
    
    def add_close_listener(self, listener):
        self.closeListeners.add(listener)
    
    def remove_close_listener(self, listener):
        self.closeListeners.remove(listener)
    
    
    def set_name(self, name):
        """sets the name of the window
           
           The name will be displayed in the window's title bar.
        """
        self.name = name
        return summon_core.set_window_name(self.winid, name)
    
    def get_name(self):
        """returns the name of a window"""
        return self.name
    
    def set_size(self, width, height):
        """sets the size of a window in pixels"""
        summon_core.set_window_size(self.winid, width, height)
    
    def get_size(self):
        """gets the size of a window in pixels"""
        return summon_core.get_window_size(self.winid)

    def set_position(self, x, y):
        """sets the position of a window on the desktop"""
        summon_core.set_window_position(self.winid, x, y)
            
    
    def get_position(self):
        """gets the position of a window on the desktop"""
        return summon_core.get_window_position(self.winid)
    
    def _on_resize(self):
        """internal callback for SUMMON's use only"""
        width, height = self.get_size()
        
        # resizing can change the view, so notify listeners
        # NOTE: focus changed must be called before view changed
        self._on_focus_change()
        self._on_view_change()
        
        for listener in self.resizeListeners:
            listener(width, height)
    
    def add_resize_listener(self, listener):
        self.resizeListeners.add(listener)
    
    def remove_resize_listener(self, listener):
        self.resizeListeners.remove(listener)
    
    def _on_move(self):
        """internal callback for SUMMON's use only"""
        x, y = self.get_position()
        
        for listener in self.moveListeners:
            listener(x, y)
    
    def add_move_listener(self, listener):
        self.moveListeners.add(listener)
    
    def remove_move_listener(self, listener):
        self.moveListeners.remove(listener)
    
    def raise_window(self, raised=True):
        """raise or lower a window above others"""
        summon_core.raise_window(self.winid, raised)
    
        
    #==================================================================
    # view

    def focus(self, x, y):
        """set the zoom focus on window position 'x, 'y'"""
        ret = summon_core.focus(self.winid, int(x), int(y))
        self._on_focus_change() # notify focus has changed
        return ret
    
    def zoom(self, x, y):
        """zoom the x- and y-axis by the factors 'x' and 'y'"""
        ret = summon_core.zoom(self.winid, x, y)    
        self._on_view_change() # notify view has changed
        return ret

    def zoomx(self, x):
        """zoom the x-axis by a factor 'x'"""
        ret = summon_core.zoomx(self.winid, x)
        self._on_view_change() # notify view has changed
        return ret
    
    def zoomy(self, y):
        """zoom the y-axis by a factor 'y'"""
        ret = summon_core.zoomy(self.winid, y)
        self._on_view_change() # notify view has changed
        return ret
    
    def zoom_camera(self, factor, factor2=None):
        """returns function that will zoom a window by a fixed factor.
           handy for use with set_binding()
           
           If two zoom factors are supplied, the x- and y-axis will be
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
        """sets the translation offset of the camera"""
        ret = summon_core.set_trans(self.winid, x, y)
        self._on_view_change() # notify view has changed
        return ret
    
    def get_trans(self):
        """gets the translation offset of the camera"""
        return summon_core.get_trans(self.winid)
    
    def set_zoom(self, x, y):
        """sets the zoom of the camera for both x- and y-axis"""
        ret = summon_core.set_zoom(self.winid, x, y)
        self._on_view_change() # notify view has changed
        return ret        

    def get_zoom(self):
        """gets the zoom of the camera for both x- and y-axis"""
        return summon_core.get_zoom(self.winid)
    
    def get_focus(self):
        """gets the zoom focus of the camera"""
        return summon_core.get_focus(self.winid)
    
    def set_focus(self, x, y):
        """sets the zoom focus of the camera"""
        ret = summon_core.set_focus(self.winid, x, y)
        self._on_focus_change() # notify focus has changed
        return ret
    
    def trans(self, x, y):
        """translates the camera by (x, y)"""
        ret = summon_core.trans(self.winid, x, y)
        self._on_view_change() # notify view has changed
        return ret        
    
    def trans_camera(self, x, y):
        """returns a function of no arguments that will translate the camera
           
           x -- units along the x-axis to translate
           y -- units along the y-axis to translate
        """
        return lambda: self.trans(x, y)
    
    def get_boundary(self):
        """gets the maximum region viewable"""
        return summon_core.get_window_boundary(self.winid)
    
    def set_boundary(self, x1, y1, x2, y2):
        """sets the maximum region viewable"""
        return summon_core.set_window_boundary(self.winid, x1, y1, x2, y2)
    
    def home(self):
        """centers the view such that all graphical elements are visible"""
        #ret = summon_core.home(self.winid)
        box = self.world.get_bounding()
        
        if box[0] > -util.INF and \
           box[1] > -util.INF and \
           box[2] < util.INF and \
           box[3] < util.INF:
            self.set_visible(*box)

    def set_visible(self, x1, y1, x2, y2, mode="one2one"):
        """sets the current view to the specified bounding box
        
        mode -- specifies the zoom using one of the following
                "one2one" sets zoom to 1:1
                "keep"    keeps zoom at current ratio
                "exact"   sets zoom exactly as bounding box implies
        """

        # ensure coordinates are properly ordered
        if x1 > x2:
            x1, x2 = x2, x1
        if y1 > y2:
            y1, y2 = y2, y1

        # do not allow empty bounding box
        if x1 == x2 or y1 == y2:
            raise Exception("can't set visible to an empty bounding box")

        # get window dimensions
        winw, winh = map(float, self.get_size())
        
        # do nothing if window has zero width or height
        if winw == 0 or winh == 0:
            return        
        
        # set visible according to mode
        if mode == "one2one":
            summon_core.set_visible(self.winid, x1, y1, x2, y2)
            
        elif mode == "keep":            
            zoomx, zoomy = summon_core.get_zoom(self.winid)
            zoomratio = zoomx / zoomy
            worldw = x2 - x1
            worldh = y2 - y1
            vieww = worldw * zoomx
            viewh = worldh * zoomy
            
            # determine which dimension is tight
            if vieww / viewh < winw / winh:
                # height is tight
                zoomy2 = winh / worldh
                zoomx2 = zoomy2 * zoomratio
                
                worldw2 = winw / zoomx2
                offset = [- (worldw2 - worldw) / 2.0, 0.0]
            else:
                # width is tight
                zoomx2 = winw / worldw
                zoomy2 = zoomx2 / zoomratio
                
                worldh2 = winh / zoomh2
                offset = [0.0, - (worldh2 - worldh) / 2.0]
            
            summon_core.set_focus(self.winid, x1 + offset[0], y1 + offset[1])
            summon_core.set_zoom(self.winid, zoomx2, zoomy2)
            summon_core.set_trans(self.winid, -x1 - offset[0], -y1 - offset[1])

            
        elif mode == "exact":
            summon_core.set_focus(self.winid, x1, y1)
            summon_core.set_zoom(self.winid, winw / float(x2 - x1), 
                                             winh / float(y2 - y1))
            summon_core.set_trans(self.winid, -x1, -y1)
            
        else:
            raise Exception("unknown zoom mode '%s'" % mode)
        
        # NOTE: focus changed must be called before view changed
        self._on_focus_change() # notify view has changed        
        self._on_view_change() # notify view has changed
        
    
    def get_visible(self):
        """returns the current view as x1, y1, x2, y2
        
           x1 -- left most x-coordinate
           y1 -- bottom most y-coordinate
           x2 -- right most x-coordinate
           y2 -- top most y-coordinate
        """
        return summon_core.get_visible(self.winid)

    def restore_zoom(self):
        """restores zoom to a 1:1 ratio"""
        
        zoomx, zoomy = self.get_zoom()

        if zoomx > zoomy:
            self.zoomy(zoomx / zoomy)
        else:
            self.zoomx(zoomy / zoomx)

    
    #======================================================================
    # view listeners
    
    def add_view_change_listener(self, listener):
        self.viewChangeListeners.add(listener)
    
    def remove_view_change_listener(self, listener):
        self.viewChangeListeners.remove(listener)
    
    def add_focus_change_listener(self, listener):
        self.focusChangeListeners.add(listener)
    
    def remove_focus_change_listener(self, listener):
        self.focusChangeListeners.remove(listener)

    def _on_view_change(self):
        """a callback for when view changes"""
        
        if self.viewLock: return
        self.viewLock = True
        
        for listener in self.viewChangeListeners:
            listener()
         
        self.viewLock = False
    
    def _on_focus_change(self):
        """a callback for when zoom focus changes"""
        
        if self.focusLock: return
        self.focusLock = True
    
        for listener in self.focusChangeListeners:
            listener()
        
        self.focusLock = False            
    
    #====================================================================
    # misc
    
    def set_bgcolor(self, r, g, b):
        """sets the backgroun color of the window"""
        return summon_core.set_bgcolor(self.winid, r, g, b)
    
    def get_bgcolor(self):
        """gets the background color of the window"""
        return summon_core.get_bgcolor(self.winid)
    
    def set_antialias(self, enabled):
        """sets whether antialiasing should be used"""
        self.antialias = enabled
        return summon_core.set_antialias(self.winid, enabled)
    
    def toggle_aliasing(self):
        """toggles the use of antialiasing"""
        self.set_antialias(not self.antialias)

    def toggle_crosshair(self):
        """toggles the visibility of the mouse crosshair"""
        self.show_crosshair(not self.crosshair)

        if self.crosshair_color == None:
            col = self.get_bgcolor()
            self.set_crosshair_color(1-col[0], 1-col[1], 1-col[2], 1)
    
    def set_crosshair_color(self, r, g, b, a=1):
        """sets the color of the mouse crosshair
        
           r -- red (0, 1)
           g -- green (0, 1)
           b -- blue (0, 1)
           a -- alpha, opacity (0,1)
        """
        self.crosshair_color = (r, g, b, a)
        return summon_core.set_crosshair_color(self.winid, r, g, b, a)

    def show_crosshair(self, enabled):
        """shows and hides the mouse crosshair"""
        self.crosshair = enabled
        return summon_core.show_crosshair(self.winid, enabled)
    
    
    #=============================================================
    # controller
    
    def add_binding(self, input_obj, func):
        """add a function to the list of functions bounded to an input"""
        
        ret = summon_core.set_binding(self.winid, input_obj, func)
        
        if isinstance(func, str):
            if func == "focus" or \
               func == "home":
                summon_core.set_binding(self.winid, input_obj, self._on_focus_change)        
        
            if func == "zoom" or \
               func == "zoomx" or \
               func == "zoomy" or \
               func == "trans" or \
               func == "home":
                summon_core.set_binding(self.winid, input_obj, self._on_view_change)
        
        return ret
        
    def clear_binding(self, input_obj):
        """clear all bindings for an input 'input_obj'"""
        return summon_core.clear_binding(self.winid, input_obj)

    def set_binding(self, input_obj, func):
        """
        bind a function 'func' to an input 'input_obj'
        
        'input_obj' must be a summon input such as input_key(), 
        input_motion(), input_click(), etc.
        """
        self.clear_binding(input_obj)
        return self.add_binding(input_obj, func)
    
    def clear_all_bindings(self):
        """clear all input bindings for the window"""
        return summon_core.clear_all_bindings(self.winid)

    def get_mouse_pos(self, coord):
        """gets the current mouse position within the window
        
           coords -- either 'world', 'screen', or 'window'.
                     mouse position will be returned as a coordinate in the
                     specified coordinate system
        """          
           
        return summon_core.get_mouse_pos(self.winid, coord)
    
    
    def hotspot_click(self, x, y):
        """performs a hotspot click at window position (x, y)"""
        summon_core.hotspot_click(self.winid, x, y)
    
    #====================================================================
    # model manipulation (forward to world)
    
    def clear_groups(self):
        """clears all groups from the world model"""
        return self.world.clear_groups()
    
    def add_group(self, aGroup):
        """adds a group to the world model""" 
        return self.world.add_group(aGroup)
    
    def insert_group(self, parentGroup, childGroup):
        """inserts a drawing group 'aGroup' as a child to an existing group
           'parentGroup'."""
        return self.world.insert_group(parentGroup, childGroup)
    
    def remove_group(self, *groups):
        """removes 'groups' from the world model"""
        return self.world.remove_group(*groups)
    
    def replace_group(self, oldGroup, newGroup):
        """replaces the group 'oldGroup' with a new group 'newGroup'"""
        return self.world.replace_group(oldGroup, newGroup)
    
    def show_group(self, aGroup, visible):
        """set the visiblity of a group in the 'world' model
           
           aGroup  -- a group or other graphical element
           visible -- a bool indicating whether the group is visible
        """    
        return self.world.show_group(aGroup, visible)
        
    def get_root(self):
        """get the root group of the window's 'world' model"""
        return self.world.get_root()


    
    #====================================================================
    # menu
    
    def set_menu_button(self, button):
        """sets the mouse button associated with the window's popup menu
        
        button can be only of the following: 'left', 'middle', 'right'
        """
        
        lookup = {"left": 0,
                  "middle": 1,
                  "right": 2}
        
        if button not in lookup:
            raise Exception("unknown mouse button '%s'" % button)
        
        self.deactivate_menu()
        self.menuButton = lookup[button]
        self.activate_menu()
    
        
    def set_menu(self, menu):
        """sets the popup menu for the window
        
        see Window.set_menu_button() for setting the popup menu mouse button
        """
        
        if menu != self.menu and self.menu != None:
            summon_core.detach_menu(self.winid, self.menu.menuid, self.menuButton)
        
        self.menu = menu
        
        self.activate_menu()
    
    def activate_menu(self):
        if self.menu != None:
            summon_core.attach_menu(self.winid, self.menu.menuid, self.menuButton)
    
    def deactivate_menu(self):
        if self.menu != None:
            summon_core.detach_menu(self.winid, self.menu.menuid, self.menuButton)
    
    #===================================================================
    # misc
    def duplicate(self):
        """returns a new window with same model and properties as this window"""
        
        return DuplicateWindow(self)
        

def copy_window_state(winSrc, winDst):
    """Copies the state of one window to another"""
    
    # get source window, model, and visible
    coords = winSrc.get_visible()
    size = winSrc.get_size()
    bg = winSrc.get_bgcolor()

    zoomx = (coords[2] - coords[0]) / size[0]
    zoomy = (coords[3] - coords[1]) / size[1]

    # make the view the same
    winDst.set_bgcolor(* bg)
    winDst.set_visible(*coords)
    coords = winDst.get_visible()
    zoomx2 = (coords[2] - coords[0]) / size[0]
    zoomy2 = (coords[3] - coords[1]) / size[1]
    winDst.focus(size[0] / 2, size[1] / 2)
    winDst.zoom(zoomx2 / zoomx, zoomy2 / zoomy)


class DuplicateWindow (Window):
    """Creates a duplicate of another window"""

    def __init__(self, win):
        Window.__init__(self, 
                        win.get_name(), 
                        position=win.get_position(),
                        size=win.get_size(), 
                        world=win.world, 
                        winconfig=win.winconfig)
        copy_window_state(win, self)


class OverviewWindow (DuplicateWindow):
    """Creates an overview of another window"""

    def __init__(self, win, frame_color=None, frame_fill=(0, 0, 1, .1)):
        DuplicateWindow.__init__(self, win)
        self.targetWin = win
        self.frame_fill = frame_fill
        self.set_name("overview of '%s'" % self.get_name())
        
        if frame_color == None:
            col = self.get_bgcolor()
            self.frame_color = (1-col[0], 1-col[1], 1-col[2], 1)
        else:
            self.frame_color = frame_color
        
        # set listeners
        win.add_view_change_listener(self.update_frame)
        self.add_view_change_listener(self.update_frame)
        win.add_close_listener(self.on_target_close)
        
        # initialize frame
        self.frameVis = group()
        self.screen.add_group(self.frameVis)
        self.update_frame()

    
    def _on_close(self):
        """Callback for when overwindow is closed"""
        self.detach()
        DuplicateWindow._on_close(self)
    
    
    def on_target_close(self):
        """Callback for when target window closes
           detach self from target.
        """
        self.detach()
    
    
    def detach(self):
        """Detach from target window"""
        
        if self.targetWin:
            if self.targetWin.is_open():
                self.targetWin.remove_view_change_listener(self.update_frame)
            self.remove_view_change_listener(self.update_frame)
            self.frameVis = self.screen.replace_group(self.frameVis, group())
            self.targetWin = None
    
    def update_frame(self):
        """Update view frame"""
    
        x1, y1, x2, y2 = self.targetWin.get_visible()
        t = self.get_trans()
        f = self.get_focus()
        z = self.get_zoom()
        
        x1, y1 = world2screen([x1, y1], t, z, f)
        x2, y2 = world2screen([x2, y2], t, z, f)
        
        vis = group(color(*self.frame_color),
                    line_strip(x1, y1, 
                               x1, y2, 
                               x2, y2, 
                               x2, y1,
                               x1, y1),
                    color(*self.frame_fill),
                    quads(x1, y1,
                          x1, y2,
                          x2, y2,
                          x2, y1))
        
        self.frameVis = self.screen.replace_group(self.frameVis, vis)
        


class Model (object):
    """A Model contains graphics for display.
    
       Each window has a 'world' and 'screen' model.
    """

    def __init__(self, modelid = None):
        if modelid == None:
            # create new window
            self.id = summon_core.new_model()
            assert self.id != None
            _state.add_model(self)
    
    # TODO: determine how to delete models
    
    def clear_groups(self):
        """clears all graphical elements from the model"""
        return summon_core.clear_groups(self.id)
    
    def add_group(self, aGroup):
        """adds a group to the model"""
        summon_core.add_group(self.id, aGroup)
        return aGroup
    
    def insert_group(self, parentGroup, childGroup):
        """inserts a drawing group 'aGroup' as a child to an existing group
           'parentGroup'."""    
        summon_core.insert_group(self.id, parentGroup.ptr, childGroup)
        return childGroup
    
    def remove_group(self, *groups):
        """removes 'groups' from the model"""
        ids = [x.ptr for x in groups]
        return summon_core.remove_group(self.id, *ids)
    
    def replace_group(self, oldGroup, newGroup):
        """replaces the group 'oldGroup' with a new group 'newGroup'"""    
        summon_core.replace_group(self.id, oldGroup.ptr, newGroup)
        return newGroup
    
    def show_group(self, aGroup, visible):
        """set the visiblity of a group
           
           aGroup  -- a group or other graphical element
           visible -- a bool indicating whether the group is visible
        """
        return summon_core.show_group(self.id, aGroup.ptr, visible)
    
    def get_root(self):
        """get the root group of the model"""
        return group(ref=summon_core.get_root_id(self.id))
    
    def get_bounding(self, aGroup=None):
        """get the bounding box for a group and its contents
        
           if aGroup is None, the root group will be used.
        """
        
        if aGroup == None:
            aGroup = self.get_root()
        return summon_core.get_bounding(self.id, aGroup.ptr)
    

#=============================================================================
# Menus


class MenuItem (object):
    def __init__(self, kind, text, action, **data):
        self.kind = kind
        self.text = text
        self.action = action
        self.data = data
    

class Menu (object):
    """SUMMON Menu
       
       This class creates pop-up menus that can be displayed by SUMMON Windows.
    """

    def __init__(self):
        self.menuid = summon_core.new_menu()
        self.items = []
    
    # NOTE: I tried the __del__ function, but had trouble with destroyMenu on
    # python exit.
    
    def delete(self):
        """delete all the resources of the menu"""
        
        # delete all items first
        self.clear()
        
        # delete menu
        summon_core.del_menu(self.menuid)
                    

    def add_entry(self, text, func):
        summon_core.add_menu_entry(self.menuid, text, func)
        self.items.append(MenuItem("entry", text, func))
    
    def add_toggle(self, text, func, state=False):
        item = MenuItem("entry", text, func, state=state)
        self.items.append(item)
    
        if state:
            summon_core.add_menu_entry(self.menuid, text + " [x]", 
                                       lambda: self._on_toggle(item))
        else:
            summon_core.add_menu_entry(self.menuid, text + " [  ]",
                                       lambda: self._on_toggle(item))

    def _on_toggle(self, item):
        index = self.items.index(item)
        
        # toggle state
        item.data["state"] = not state
        
        # update menu
        if item.data["state"]:
            self.set_entry(index, item.text + " [x]", 
                           lambda: self._on_toggle(item))
        else:
            self.set_entry(index, item.text + " [  ]", 
                           lambda: self._on_toggle(item))
        
        # call function
        item.action()
    
    def add_submenu(self, text, submenu):
        summon_core.add_submenu(self.menuid, text, submenu.menuid)
        self.items.append(MenuItem("submenu", text, submenu))
    
    def get_item(self, index):
        return self.items[index]
    
    def set_item(self, index, item):
        if item.kind == "entry":
            summon_core.set_menu_entry(self.menuid, index+1, item.text, item.action)
        elif item.kind == "submenu":
            summon_core.set_submenu(self.menuid, index+1, item.text, item.action.menuid)
        else:
            raise Exception("unknown menu item '%s'" % item.kind)
        
        self.items[index] = item
    
    def add_item(self, item):
        if item.kind == "entry":
            summon_core.add_menu_entry(self.menuid, item.text, item.action)
        elif item.kind == "submenu":
            summon_core.add_submenu(self.menuid, item.text, item.action.menuid)
        else:
            raise Exception("unknown menu item '%s'" % item.kind)
        
        self.items.append(item)
    
    def set_entry(self, index, text, func):
        summon_core.set_menu_entry(self.menuid, index+1, text, func)
        self.items[index] = MenuItem("entry", text, func)
    
    def set_submenu(self, index, text, submenu):
        summon_core.set_submenu(self.menuid, index+1, text, submenu.menuid)
        self.items[index] = MenuItem("submenu", text, submenu)
    
    def insert_entry(self, index, text, func):
        self._move_items_down(index)
        self.set_entry(index, text, func)
    
    def insert_submenu(self, index, text, submenu):
        self._move_items_down(index)
        self.set_submenu(index, text, submenu)
    
    def insert_item(self, index, item):
        self._move_items_down(index)
        self.set_item(index, item)
    
    def _move_items_down(self, index):
        index2 = len(self.items)
        if index2 == 0:
            return
        
        # copy last item down
        item = self.get_item(index2-1)
        self.add_item(item)
        
        # move down the rest
        for i in xrange(index2-2, index-1, -1):
            self.set_item(i+1, self.get_item(i))
        
        
    
    def remove(self, index):
        """removes a menu item at position 'index'"""
    
        summon_core.remove_menu_item(self.menuid, index+1)
        del self.items[index]
    
    def clear(self):
        """clears all menu items"""
        
        for i, item in enumerate(self.items):
            if not isinstance(item[1], Menu):
                summon_core.remove_menu_item(self.menuid, i+1)
    


class SummonMenu (Menu):
    """Default SUMMON menu"""
    
    def __init__(self, win):
        Menu.__init__(self)
        
        # window options
        self.window_menu = Menu()
        self.window_menu.add_entry("duplicate   (ctrl+d)", win.duplicate)        
        self.window_menu.add_entry("overview   (ctrl+o)", lambda: OverviewWindow(win))
        self.add_submenu("Window", self.window_menu)

        # zoom
        self.zoom_menu = Menu()
        self.zoom_menu.add_entry("home   (h)", win.home)
        self.zoom_menu.add_entry("zoom 1:1   (ctrl+h)", win.restore_zoom)
        self.add_submenu("Zoom", self.zoom_menu)

        # print screen options
        self.print_screen_menu = Menu()
        self.print_screen_menu.add_entry("svg   (ctrl+p)", 
            lambda: summon.svg.printScreen(win))
        self.print_screen_menu.add_entry("png   (ctrl+P)", 
            lambda: summon.svg.printScreenPng(win))
        self.add_submenu("Print screen", self.print_screen_menu)        

        # misc
        self.misc = Menu()
        self.misc.add_entry("toggle crosshair  (ctrl+x)", win.toggle_crosshair)
        self.misc.add_entry("toggle aliasing   (ctrl+l)", win.toggle_aliasing)
        self.add_submenu("Misc", self.misc)

        self.add_entry("close   (q)", win.close)


#=============================================================================
# Dynamic updating 

class VisObject (object):
    """Base class of dynamic visualization objects"""
    
    def __init__(self):
        self.win = None
        self.__timer = None
    
    def __del__(self):
        if _state != None and self.__timer != None:
            self.__timer.stop()
    
    def update(self):
        """this function is called from the SUMMER timer"""
        pass
    
    def show(self):
        pass
    
    def enable_updating(self, enable=True, interval=None):
        """start/stop this object's update function"""
        
        if enable:
            self.__timer = add_timer(self.update, interval, True, self.win)
        elif self.timer != None:
            self.__timer.stop()   
    
    
    def get_window(self):
        return self.win
    
    def set_window(self, win):
        self.win = win




#=============================================================================
# coordinate system conversions


def world2screen(pt, trans, zoom, focus):
    """converts a world point to a screen point"""
    return [trans[0] + focus[0] + zoom[0] * (pt[0] - focus[0]),
            trans[1] + focus[1] + zoom[1] * (pt[1] - focus[1])]


def window2screen(pt, win_height):
    """converts a window point to a screen point"""
    return Vertex2i(pt[0], win_height - pt[1])

def screen2world(pt, trans, zoom, focus):
    """converts a screen point to a world point"""
    return [(pt[0] - trans[0] - focus[0]) / zoom[0] + focus[0],
            (pt[1] - trans[1] - focus[1]) / zoom[1] + focus[1]]


def window2world(pt, trans, zoom, focus, win_height):
    """converts a window to a world point"""
    return [(pt[0] - trans[0] - focus[0]) / zoom[0] + focus[0],
            (win_height - p[1] - trans[1] - focus[1]) / zoom[1] + focus[1]]


#=============================================================================
# functions for iterating and inspecting graphical elements
#

def iter_vertices(elm, curcolor=None):
    """iterates through the vertices of a graphic"""

    if curcolor == None:
        curcolor = [1.0, 1.0, 1.0, 1.0]
    verts = []
    
    # iterate over primitives
    for prim in elm.get_contents():
        if is_color(prim):
            # color primitive
            rgb = list(prim[1:])
            if len(rgb) == 3:
                rgb.append(1.0)
            
            # update current color
            curcolor = rgb

        elif is_vertices(prim):
            # vertices primitive
            coords = prim[1:]
            
            for i in xrange(0, len(coords), 2):
                # TODO: add triangle_fan()
            
                # push a new vertex                    
                verts.append(coords[i:i+2])

                if isinstance(elm, points):            nverts, nkeep = 1, 0
                elif isinstance(elm, lines):           nverts, nkeep = 2, 0
                elif isinstance(elm, line_strip):      nverts, nkeep = 2, 1
                elif isinstance(elm, triangles):       nverts, nkeep = 3, 0
                elif isinstance(elm, triangle_strip):  nverts, nkeep = 3, 2
                elif isinstance(elm, quads):           nverts, nkeep = 4, 0
                elif isinstance(elm, quad_strip):      nverts, nkeep = 4, 2
                else:
                    nverts, keep = util.INF, 0

                # yield vertices, when the appropriate number of vertices have 
                # been collected
                if len(verts) == nverts:
                    yield verts, curcolor
                    if nkeep == 0:
                        verts = []
                    else:
                        verts = verts[-nkeep:]
            
            if isinstance(elm, polygon):
                yield verts, curcolor
    
    if isinstance(elm, color_graphic):
        yield [], curcolor
    




#=============================================================================
# additonal modules imported here
#

from summon.colors import *
