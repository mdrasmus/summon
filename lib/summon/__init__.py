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
from summon import util
import summon.svg


VERSION = "1.8.1"
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

def timer_call(delay, func):
    """calls a function 'func' after 'delay' seconds have past"""
    summon_core.timer_call(delay, func)


#=============================================================================
# Dynamic updating interface
#



class FunctionTimer:
    def __init__(self, func, win, interval=None, repeat=True):
        self.func = func
        self.win = win
        self.interval = interval
        self.repeat = True
        self.delay = interval


class SummonTimer:
    """
    SUMMON provides a timer that calls a python function of your choice at
    regular intervals.  The following functions provide an interface to the
    SUMMON timer.

    """

    def __init__(self):
        self.updateFuncs = []
        self.updateInterval = .5
        self.timestep = 0
        
    
    def add_update_func(self, func, win, interval=None):
        """
        adds a function of no arguments to the list of functions called by SUMMON
        at regular intervals
        """
        if interval == None:
            interval = self.updateInterval
        self.updateFuncs.append(FunctionTimer(func, win, interval))

    def remove_update_func(self, func):
        """
        removes a function from the list of SUMMON timer functions
        """
        self.updateFuncs = filter(lambda x: x.func != func, self.updateFuncs)
    
    
    def is_update_func(self, func):
        """returns True if 'func' is currently registered as a SUMMON timer function"""
        return func in (x.func for x in self.updateFuncs)


    def get_update_funcs(self):
        """returns all functions registered as SUMMON timer functions"""
        return self.updateFuncs

    
    def set_update_interval(self, secs):
        """sets the timer interval between calls the to the timer function"""
        self.updateInterval = secs

    
    def begin_updating(self):
        """starts the SUMMON timer"""
        
        windows = set(summon_core.get_windows())
        dels = set()
        
        # call each timer that has past
        mindelay = util.INF
        for timer in self.updateFuncs:
            if timer.win.winid in windows:
                timer.delay -= self.timestep
                
                if timer.delay <= 0:
                    timer.func()
                    if timer.repeat:
                        timer.delay = timer.interval
                    else:
                        dels.add(timer)
                
                mindelay = min(mindelay, timer.delay)
            else:
                # remove functions of closed windows
                dels.add(timer)

        # remove closed windows
        if len(dels) > 0:
            self.updateFuncs = filter(lambda x: x not in dels, self.updateFuncs)
        
        
        # setup next call
        #self.timestep = self.updateInterval
        #timer_call(self.updateInterval, self.begin_updating)
        
        if mindelay < util.INF:
            self.timestep = mindelay
            timer_call(self.timestep, self.begin_updating)
    
    
    def stop_updating(self):
        """stops the SUMMON timer"""
        summon_core.timer_call(0, lambda :None)



##
# module function interface
#    

def add_update_func(func, win, interval=None):
    """
    adds a function of no arguments to the list of functions called by SUMMON
    at regular intervals
    """
    state.timer.add_update_func(func, win, interval)


def remove_update_func(func):
    """
    removes a function from the list of SUMMON timer functions
    """
    state.timer.remove_update_func(func)


def is_update_func(func):
    """returns True if 'func' is currently registered as a SUMMON timer function"""
    return state.timer.is_update_func(func)


def get_update_funcs():
    """returns all functions registered as SUMMON timer functions"""
    return state.timer.updateFuncs

def set_update_interval(secs):
    """sets the timer interval between calls the to the timer function"""
    state.timer.set_update_interval(secs)


def begin_updating():
    """starts the SUMMON timer"""
    state.timer.begin_updating()
    
def stop_updating():
    """stops the SUMMON timer"""
    state.timer.stop_updating()



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
        self.timer = SummonTimer()
        
    
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

# global singleton
state = SummonState()


def get_summon_state():
    """returns the summon state singleton"""
    return _summon_state


def get_summon_window():
    """returns the currently active summon window"""
    return state.current_window

# install summon window close callback for communication between C++ and python
def _window_close_callback(winid):
    state.windows[winid]._on_close()
summon_core.set_window_close_callback(_window_close_callback)

_window_decoration = summon_core.get_window_decoration()
def get_window_decoration():
    return _window_decoration

def set_window_decoration(xoffset, yoffset):
    global _window_decoration
    _window_decoration = (xoffset, yoffset)


#=============================================================================
# Visdraw window objects
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
       be whatever the user wants.  The size and orientation of the world  model
       is dependent on scrolling and zooming. The world model is the default
       model that is  used when drawing (e.g. functions such as Window.add_group
       forward their arguments to Window.world.add_group).

       In contrast, the screen model is always drawn with its origin at the 
       lower left corner of the window and with units in terms of pixels. The
       screen model is always drawn on top of the world model.  The screen model
       is often used to draw menus, toolbars, or other things that should always
       be in view, and should not be affected by scrolling and zooming.
    """

    def __init__(self, name="SUMMON", worldid=None, screenid=None,
                 loadconfig=True):
        # create new window
        self.winid = summon_core.new_window()
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
        
        summon_core.set_window_on_resize(self.winid, self._on_resize)
        
        
        self.set_name(name)
        self.antialias = True
        self.crosshair = False
        self.crosshair_color = None
        
        # listeners
        self.viewChangeListeners = set()
        self.focusChangeListeners = set()
        self.closeListeners = set()
        self.viewLock = False
        self.focusLock = False
        
        # menu
        self.menu = None
        self.menuButton = 1  # middle mouse button
        
        # load default configuration
        if loadconfig:
            self.load_config()
    
    
    def set_world_model(self, model):
        """sets a model to be the world model"""
        summon_core.assign_model(self.winid, "world", model.id)
        self.world = model
    
     
    def set_screen_model(self, model):
        """sets a model to be the world model"""
        summon_core.assign_model(self.winid, "screen", model.id)
        self.screen = model
           
    
    def load_config(self):
        """loads a summon config file for a window"""
        state.current_window = self
        
        # look for config in HOME directory
        if "HOME" in os.environ:
            config_file = os.path.join(os.environ["HOME"], ".summon_config")
            if os.path.exists(config_file):
                execfile(config_file)
                return
        
        # try load config from python path
        try:
            import summon_config
            reload(summon_config)
        except Exception, e:
            print "Warning: could not import summon_config"
            print e

    #===========================================================
    # view
    def is_open(self):
        """returns whether underling SUMMON window is open"""
        
        return self.winid in state.windows
    
    def get_decoration(self):
        """returns the window decoration size (width, height)
           
           Decoration size refers to the 'width' of the side frame and the
           'height' of the title bar.
        """
        return _window_decoration    
    

    def close(self):
        """close the window"""
        ret = summon_core.close_window(self.winid)
        # self._on_close() will get called back through the C++ module
        # letting us know the window is truely closed
        return ret
    
    def _on_close(self):
        """a callback for window close events"""
        
        # let the global summon state know, that this window is closed
        state.remove_window(self)
        
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
        return summon_core.set_window_size(self.winid, width, height)
    
    def get_size(self):
        """gets the size of a window in pixels"""
        return summon_core.get_window_size(self.winid)

    def set_position(self, x, y):
        """sets the position of a window on the desktop"""
        return summon_core.set_window_position(self.winid, x, y)
    
    def get_position(self):
        """gets the position of a window on the desktop"""
        return summon_core.get_window_position(self.winid)
    
    def _on_resize(self):
        return self.on_resize(* self.get_size())
    
    def on_resize(self, width, height):
        """a callback for when the window resizes"""
        pass
        
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

    
    def add_view_change_listener(self, listener):
        self.viewChangeListeners.add(listener)
    
    def remove_view_change_listener(self, listener):
        self.viewChangeListeners.remove(listener)
    
    def add_focus_change_listener(self, listener):
        self.focusChangeListeners.add(listener)
    
    def remove_focus_change_listener(self, listener):
        self.focusChangeListeners.remove(listener)
    
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
    
    def home(self):
        """centers the view such that all graphical elements are visible"""
        #ret = summon_core.home(self.winid)
        box = self.world.get_bounding()
        self.set_visible(*box)

    def set_visible(self, x1, y1, x2, y2):
        """sets the current view to the specified bounding box"""
        ret = summon_core.set_visible(self.winid, x1, y1, x2, y2)
        self._on_focus_change() # notify view has changed        
        self._on_view_change() # notify view has changed
        return ret
    
    def get_visible(self):
        """returns the current view as x1, y1, x2, y2
        
           x1 -- left most x-coordinate
           y1 -- bottom most y-coordinate
           x2 -- right most x-coordinate
           y2 -- top most y-coordinate
        """
        return summon_core.get_visible(self.winid)

        
    
    #====================================================================
    
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
        """bind a function 'func' to an input 'input_obj'"""
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
    
    # DEPRECATED
    def get_root_id(self):
        """get the root group of the window's 'world' model"""
        return self.world.get_root_id()
    
    
    #====================================================================
    # menu
    
    def set_menu_button(self, button):
        lookup = {"left": 0,
                  "middle": 1,
                  "right": 2}
        
        if button not in lookup:
            raise Exception("unknown mouse button '%s'" % button)
        
        self.menuButton = lookup[button]
    
        
    def set_menu(self, menu):
        if menu != None:
            summon_core.attach_menu(self.winid, menu.menuid, self.menuButton)
        elif self.menu != None:
            summon_core.detach_menu(self.winid, self.menu.menuid, self.menuButton)
        
        self.menu = menu

    
    
    #===================================================================
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
        win.set_position(* self.get_position())
        win.set_name(self.get_name())
        win.set_bgcolor(* bg)
        
        # make the view the same
        win.set_visible(*coords)
        coords = win.get_visible()
        zoomx2 = (coords[2] - coords[0]) / size[0]
        zoomy2 = (coords[3] - coords[1]) / size[1]
        win.focus(size[0] / 2, size[1] / 2)
        win.zoom(zoomx2 / zoomx, zoomy2 / zoomy)
        
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
        return group(summon_core.get_root_id(self.id), ref=True)
    
    def get_bounding(self, aGroup=None):
        """get the bounding box for a group and its contents
        
           if aGroup is None, the root group will be used.
        """
        
        if aGroup == None:
            aGroup = self.get_root()
        return summon_core.get_bounding(self.id, aGroup.ptr)
    
    
    # DEPRECATED
    def get_root_id(self):
        return group(summon_core.get_root_id(self.id), ref=True)




class Menu:
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
        self.items.append([text, func])
    
    def add_toggle(self, text, func, state=False):
        item = [text, func, state]
        self.items.append(item)
    
        if state:
            summon_core.add_menu_entry(self.menuid, text + " [x]", 
                                       lambda: self._on_toggle(item))
        else:
            summon_core.add_menu_entry(self.menuid, text + " [  ]",
                                       lambda: self._on_toggle(item))

    def _on_toggle(self, item):
        index = self.items.index(item)
        text, func, state = item
        
        # toggle state
        state = not state
        item[2] = state
        
        # update menu
        if state:
            self.set_entry(index, text + " [x]", lambda: self._on_toggle(item))
        else:
            self.set_entry(index, text + " [  ]", lambda: self._on_toggle(item))
        
        # call function
        func()
    
    def add_submenu(self, text, submenu):
        summon_core.add_submenu(self.menuid, text, submenu.menuid)
        self.items.append([text, submenu])
    
    def get_item(self, index):
        return self.items[index]
    
    def set_entry(self, index, text, func):
        summon_core.set_menu_entry(self.menuid, index+1, text, func)
    
    def set_submenu(self, index, text, submenu):
        summon_core.set_submenu(self.menuid, index+1, text, submenu.menuid)
    
    def remove(self, index):
        """removes a menu item at position 'index'"""
    
        summon_core.remove_menu_item(self.menuid, index+1)
        
        # remove item from internal list
        olditems = self.items
        self.items = []
        for i in xrange(len(olditems)):
            if i != index:
                self.items.append(olditems[i])
    
    def clear(self):
        """clears all menu items"""
        
        for i, item in enumerate(self.items):
            if not isinstance(item[1], Menu):
                summon_core.remove_menu_item(self.menuid, i+1)    


class SummonMenu (Menu):
    """Default SUMMON menu"""
    
    def __init__(self, win):
        Menu.__init__(self)
        
        self.add_entry("home   (h)", win.home)        
        self.add_toggle("crosshair   (ctrl+x)", win.toggle_crosshair, False)
        self.add_toggle("smooth   (ctrl+l)", win.toggle_aliasing, True)
        self.add_entry("duplicate window   (ctrl+d)", win.duplicate)
        
        self.print_screen_menu = Menu()
        self.print_screen_menu.add_entry("svg   (ctrl+p)", 
            lambda: summon.svg.printScreen(win))
        self.print_screen_menu.add_entry("png   (ctrl+P)", 
            lambda: summon.svg.printScreenPng(win))
        self.add_submenu("print screen", self.print_screen_menu)
        
        self.add_entry("close   (q)", win.close)


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
    
    def enableUpdating(self, visible=True, interval=None):
        """add/remove this object's update function to/from the SUMMON timer"""
        if visible:    
            if not is_update_func(self.update):
                assert self.win != None, "must set window"
                add_update_func(self.update, self.win, interval)
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
                elif isinstance(elm,  triangle_strip): nverts, nkeep = 3, 2
                elif isinstance(elm, quads):           nverts, nkeep = 4, 0
                elif isinstance(elm, quad_strip):      nverts, nkeep = 4, 2

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
    


'''


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

'''



#=============================================================================
# additonal modules imported here
#

from summon.colors import *
