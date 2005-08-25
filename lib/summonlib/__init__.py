#
# SUMMON
# Matt Rasmussen
# __init__.py  summonlib package, basic python extensions to SUMMON core
#


from summon import *



#
# state of visdraw
#
class State:
    def __init__(self):
        self.antialias = True
        self.updateFuncs = []
        self.updateInterval = .5

state = State()

#
# common functions needed for key bindings
#
def trans_camera(x, y):
    def func():
        trans(x, y)
    return func

def zoom_camera(factor):
    def func():
        (w, h) = get_window_size()
        focus(w/2, h/2)
        zoom(factor, factor)
    return func

def toggle_aliasing():
    state.antialias = not state.antialias
    set_antialias(state.antialias)


#
# common editing functions
#


def push_transform(gid, trans, * args):
    grp = group()
    args = list(args) + [grp]
    insert_group(gid, group(trans(* args)))
    return get_group_id(grp)

def init_binding(input_obj, func):
    clear_binding(input_obj)
    set_binding(input_obj, func)


#
# Dynamic updating interface
#

def set_update_interval(secs):
    state.updateInterval = secs

def add_update_func(func):
    state.updateFuncs.append(func)

def get_update_funcs():
    return state.updateFuncs

def begin_updating():
    for func in state.updateFuncs:
        func()
    timer_call(state.updateInterval, begin_updating)
    
    

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
        return translate_content(elm)[2:]
    elif is_scale(elm):
        return scale_contents(elm)[2:]
    elif is_flip(elm):
        return flip_contents(elm)[2:]
    elif is_rotate(elm):
        return rotate_contents(elm)[1:]
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
    def __init__(self, winid = None):
        if winid == None:
            # create new window
            self.winid = new_window()
            self.worldid = new_model()
            self.screenid = new_model()
            
            assign_model(self.winid, "world", self.worldid)
            assign_model(self.winid, "screen", self.screenid)
        else:
            # attach to existing window
            self.winid = winid
            self.worldid = get_model(winid, "world")
            self.screenid = get_model(winid, "screen")
        

        
        # load default configuration
        oldid = get_window()
        set_window(self.winid)
        
        self.cursor = get_root_id()
        
        import summon_config
        reload(summon_config)
        set_window(oldid)
        
        

    def activate(self):
        set_window(self.winid)

    def close(self):
        close_window(self.winid)

    def apply(self, func, *args):
        oldwin = get_window()
        oldmodel = get_model(oldwin, "world")
        
        set_window(self.winid)
        set_model(self.worldid)
        apply(func, args)
        set_window(oldwin)
        set_model(oldmodel)
    
    
    def add_group(self, *args):
        set_model(self.worldid)
        return apply(add_group, args)
    
    def insert_group(self, *args):
        set_model(self.worldid)
        return apply(insert_group, args)
    
    def remove_group(self, *args):
        set_model(self.worldid)
        return apply(remove_group, args)
    
    def replace_group(self, *args):
        set_model(self.worldid)
        return apply(replace_group, args)
    
    
    

def dupWindow():
    import summon_config

    # get current window, model, and visible
    cur = get_window()
    model = get_model(cur, "world")
    coords = get_visible()

    # create new window with same model and coords    
    win = new_window()
    assign_model(win, "world", model)
    set_window(win)
    set_visible(*coords)
    reload(summon_config)
    set_window(cur)

def defaultWindow():
    return Window(get_window())


