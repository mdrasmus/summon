#
# SUMMON
# Matt Rasmussen
# __init__.py  summon package, basic python extensions to SUMMON core
#

import os, sys
from summon.core import *
from summon import util

state = get_summon_state()



def load_config():
    """Load a summon config file"""
    
    # look for config in HOME directory
    config_file = os.path.join(os.environ["HOME"], ".summon_config")
    if not os.path.exists(config_file):
        # loof for config in python paths
        try:
            import summon_config
            reload(summon_config)
        except Exception, e:
            print "could not import summon_config"
            print e
    
    else:
        execfile(config_file)

#
# common editing functions
#


def push_transform(gid, trans, * args):
    grp = group()
    args = list(args) + [grp]
    insert_group(gid, group(trans(* args)))
    return get_group_id(grp)



#
# Dynamic updating interface
#

# new python implementation of timer_call()
def set_update_interval(secs):
    state.updateInterval = secs

def add_update_func(func, win):
    state.updateFuncs.append((func, win))

def remove_update_func(func):
    state.updateFuncs = filter(lambda x: x[0] != func, state.updateFuncs)

def is_update_func(func):
    return func in util.cget(state.updateFuncs, 0)

def get_update_funcs():
    return state.updateFuncs

def begin_updating():
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
    timer_call(0, lambda :None)

#
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


#
# Visdraw window objects
#

class Window:
    """The SUMMON Window"""

    def __init__(self, name="SUMMON", winid = None, worldid=None, screenid=None):
        if winid == None:
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
            
            # load default configuration
            self.activate()
            load_config()
            
            self.set_name(name)
    
    
    def set_name(self, name):
        self.name = name
        return set_window_name(self.winid, name)
    
    def activate(self):
        summon_core.set_window(self.winid)
        state.current_window = self
        

    def close(self):
        state.remove_window(self)
        return summon_core.close_window(self.winid)
    
    
    def duplicate(self):
        # get current window, model, and visible
        coords = self.get_visible()
        size = self.get_window_size()
        bg = self.get_bgcolor()

        zoomx = (coords[2] - coords[0]) / size[0]
        zoomy = (coords[3] - coords[1]) / size[1]

        # create new window with same model and coords    
        win = Window(worldid=self.world.id)
        win.set_window_size(* size)
        win.set_bgcolor(* bg)
        
        # make the view the same
        win.set_visible(*coords)
        coords = win.get_visible()
        zoomx2 = (coords[2] - coords[0]) / size[0]
        zoomy2 = (coords[3] - coords[1]) / size[1]
        win.focus(size[0] / 2.0, size[1] / 2.0)
        win.zoom(zoomx2 / zoomx, zoomy2 / zoomy)
        
        
        # load up the configuration
        win.activate()
        load_config()
        
        return win
    
    
    # model manipulation (forward to world)
    def clear_groups(self):
        return self.world.clear_groups()
    
    def add_group(self, *args):
        return self.world.add_group(*args)
    
    def insert_group(self, *args):
        return self.world.insert_group(*args)
    
    def remove_group(self, *args):
        return self.world.remove_group(*args)
    
    def replace_group(self, *args):
        return self.world.replace_group(*args)
    
    def show_group(self, *args):
        return self.world.show_group(*args)
    
    def get_group(self, *args):
        return self.world.get_group(*args)
    
    def get_root_id(self):
        return self.world.get_root_id()
    
    
    # view
    def set_size(self, width, height):
        return summon_core.set_window_size(self.winid, width, height)
    set_window_size = set_size
    
    def get_size(self):
        return summon_core.get_window_size(self.winid)
    get_window_size = get_size

    def set_position(self, x, y):
        return summon_core.set_window_position(self.winid, x, y)
    
    def get_position(self):
        return summon_core.get_window_position(self.winid)
    
    def focus(self, x, y):
        set_window(self.winid)
        return focus(x, y)
    
    def zoom(self, x, y):
        set_window(self.winid)
        return zoom(x, y)

    def zoomx(self, x):
        set_window(self.winid)
        return zoomx(x)
    
    def zoomy(self, y):
        set_window(self.winid)
        return zoomy(y)

    def zoom_camera(self, factor, factor2=None):
        if factor2 == None:
            factor2 = factor

        def func():
            w, h = self.get_window_size()
            self.focus(w/2, h/2)
            self.zoom(factor, factor2)
        return func
    
    def set_trans(self, x, y):
        return summon_core.set_trans(self.winid, x, y)
    
    def get_trans(self):
        return summon_core.get_trans(self.winid)
    
    def set_zoom(self, x, y):
        return summon_core.set_zoom(self.winid, x, y)

    def get_zoom(self):
        return summon_core.get_zoom(self.winid)
    
    def get_focus(self):
        return summon_core.get_focus(self.winid)
    
    def set_focus(self, x, y):
        return summon_core.set_focus(self.winid, x, y)
    
    def trans(self, x, y):
        set_window(self.winid)
        return trans(x, y)
    
    def trans_camera(self, x, y):
        """Return a function of no arguments that will translate the camera"""
        return lambda: self.trans(x, y)
    
    def home(self):
        return summon_core.home(self.winid)
    
    def set_bgcolor(self, *args):
        return summon_core.set_bgcolor(self.winid, *args)
    
    def get_bgcolor(self, *args):
        return summon_core.get_bgcolor(self.winid, *args)
    
    def set_visible(self, x1, y1, x2, y2):
        return summon_core.set_visible(self.winid, x1, y1, x2, y2)
    
    def get_visible(self):
        return summon_core.get_visible(self.winid)
    
    def set_antialias(self, *args):
        return summon_core.set_antialias(self.winid, *args)
    
    def toggle_aliasing(self):
        state.antialias = not state.antialias
        self.set_antialias(state.antialias)

    def toggle_crosshair(self):
        state.crosshair = not state.crosshair
        self.show_crosshair(state.crosshair)

        if state.crosshair_color == None:
            col = self.get_bgcolor()
            self.set_crosshair_color(1-col[0], 1-col[1], 1-col[2], 1)
    
    def set_crosshair_color(self, r, g, b, a=1):
        state.crosshair_color = (r, g, b, a)
        return summon_config.set_crosshair_color(self.winid, r, g, b, a)

    def show_crosshair(self, enabled):
        state.crosshair = enabled
        return summon_config.show_crosshair(self.winid, enabled)

    
    
    # controller
    def add_binding(self, input_obj, func):
        return summon_core.set_binding(self.winid, input_obj, func)
        
    def clear_binding(self, input_obj):
        return summon_core.clear_binding(self.winid, input_obj)

    def set_binding(self, input_obj, func):
        summon_core.clear_binding(self.winid, input_obj)
        return summon_core.set_binding(self.winid, input_obj, func)
    reset_binding = set_binding
    
    def clear_all_bindings(self):
        return summon_core.clear_all_bindings(self.winid)

    def get_mouse_pos(self, coord):
        return summon_core.get_mouse_pos(self.winid, coord)        
                
    
class Model:
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
    
    def add_group(self, *groups):
        return summon_core.add_group(self.id, *groups)
    
    def insert_group(self, groupid, *groups):
        return summon_core.insert_group(self.id, groupid, *groups)
    
    def remove_group(self, groupids):
        return summon_core.remove_group(self.id, *groupids)
    
    def replace_group(self, groupid, *groups):
        return summon_core.replace_group(self.id, groupid, *groups)
    
    def get_root_id(self):
        return summon_core.get_root_id(self.id)

    def show_group(self, groupid, visible):
        return summon_core.show_group(self.id, groupid, visible)
    
    def get_group(self, groupid):
        return summon_core.get_group(self.id, groupid)




class VisObject (object):
    """Base class of dynamic visualization objects"""
    def __init__(self):
        self.win = None
    
    def __del__(self):
        self.setVisible(False)
    
    def update(self):
        pass
    
    def show(self):
        pass
    
    def enableUpdating(self, visible=True):
        if visible:    
            if not is_update_func(self.update):
                assert self.win != None, "must set window"
                add_update_func(self.update, self.win)
        else:
            if summon.is_update_func(self.update):
                remove_update_func(self.update)

    setVisible = enableUpdating
    


