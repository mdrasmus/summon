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
        except:
            print "could not import summon_config"
    
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

    def __init__(self, name="SUMMON", winid = None):
        if winid == None:
            # create new window
            self.winid = new_window()
            self.world = Model()
            self.screen = Model()
            self.name = name

            assert self.winid != None
            
            set_window_name(self.winid, name)
            
            assign_model(self.winid, "world", self.world.id)
            assign_model(self.winid, "screen", self.screen.id)
            
            # load default configuration
            set_window(self.winid)
            set_model(self.world.id)

            self.cursor = get_root_id()
            
            self.activate()
            load_config()

        else:
            # attach to existing window
            self.winid = winid
            self.world = Model(get_model(winid, "world"))
            self.screen = Model(get_model(winid, "screen"))
            
            self.activate()
        
    
    
    def set_name(self, name):
        self.name = name
        return set_window_name(self.winid, name)
    
    def activate(self):
        set_window(self.winid)
        set_model(self.world.id)
        state.current_window = self
        

    def close(self):
        return close_window(self.winid)

    def apply(self, func, *args):
        set_window(self.winid)
        set_model(self.world.id)
        return func(*args)
    
    
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

    def trans(self, x, y):
        set_window(self.winid)
        return trans(x, y)
    
    def home(self):
        set_window(self.winid)
        return home()
    
    def set_bgcolor(self, *args):
        set_window(self.winid)
        return set_bgcolor(*args)
    
    def get_bgcolor(self, *args):
        set_window(self.winid)    
        return get_bgcolor(*args)
    
    def set_visible(self, x1, y1, x2, y2):
        set_window(self.winid)
        return set_visible(x1, y1, x2, y2)
    
    def get_visible(self):
        set_window(self.winid)
        return get_visible()
        
    def set_window_size(self, width, height):
        set_window(self.winid)
        return set_window_size(width, height)
    
    def get_window_size(self):
        set_window(self.winid)
        return get_window_size()    
    
    def set_antialias(self, *args):
        set_window(self.winid)
        return set_antialias(*args)
    
    def show_crosshair(self, *args):
        set_window(self.winid)
        return show_crosshair(*args)

    def set_crosshair_color(self, *args):
        set_window(self.winid)
        return set_crosshair_color(* args)
    
    
    # controller
    def set_binding(self, *args):
        set_window(self.winid)
        return set_binding(*args)
        
    def clear_binding(self, *args):
        set_window(self.winid)
        return clear_binding(*args)

    def reset_binding(self, *args):
        set_window(self.winid)
        return reset_binding(*args)
    
    def clear_all_bindings(self):
        set_window(self.winid)
        return clear_all_bindings()

    def get_mouse_pos(self, coord):
        set_window(self.winid)
        return get_mouse_pos(coord)        
                
    
class Model:
    """A Model contains graphics for display.
    
       Each window has a 'world' and 'screen' model.
    """

    def __init__(self, modelid = None):
        if modelid == None:
            # create new window
            self.id = new_model()
            assert self.id != None
        else:
            # attach to existing window
            self.id = modelid
    
    
    def clear_groups(self):
        set_model(self.id)
        return clear_groups()
    
    def add_group(self, *args):
        set_model(self.id)
        return add_group(*args)
    
    def insert_group(self, *args):
        set_model(self.id)
        return insert_group(*args)
    
    def remove_group(self, *args):
        set_model(self.id)
        return remove_group(*args)
    
    def replace_group(self, *args):
        set_model(self.id)
        return replace_group(*args)
    
    def get_root_id(self):
        set_model(self.id)
        return get_root_id()

    def show_group(self, *args):
        set_model(self.id)
        return show_group(*args)
    
    def get_group(self, *args):
        set_model(self.id)
        return get_group(*args)

    


def dupWindow(cur=None):
    # get current window, model, and visible
    if cur == None:
        cur = get_window()
    model = get_model(cur, "world")
    coords = get_visible()
    size = get_window_size()
    bg = get_bgcolor()

    zoomx = (coords[2] - coords[0]) / size[0]
    zoomy = (coords[3] - coords[1]) / size[1]

    # create new window with same model and coords    
    win = new_window()
    assign_model(win, "world", model)
    set_window(win)
    set_window_size(* size)
    set_bgcolor(* bg)
        
    set_visible(*coords)
    coords = get_visible()
    
    zoomx2 = (coords[2] - coords[0]) / size[0]
    zoomy2 = (coords[3] - coords[1]) / size[1]
    
    focus(size[0] / 2.0, size[1] / 2.0)
    zoom(zoomx2 / zoomx, zoomy2 / zoomy)
    
    load_config()
    set_window(cur)

def defaultWindow():
    return Window(get_window())





