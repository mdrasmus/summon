from summon import *
import sys


commands = """
add_group
assign_model
call_proc
clear_all_bindings
clear_binding
clear_groups
close_window
del_model
focus
get_bgcolor
get_group
get_group_id
get_model
get_models
get_mouse_pos
get_root_id
get_visible
get_window
get_window_name
get_window_size
get_windows
home
insert_group
new_groupid
new_model
new_window
redraw_call
remove_group
replace_group
reset_binding
set_antialias
set_bgcolor
set_binding
set_crosshair_color
set_model
set_visible
set_window
set_window_name
set_window_size
show_crosshair
show_group
timer_call
toggle_aliasing
toggle_crosshair
trans
version
vertices
zoom
zoomx
zoomy
""".split()
commands.sort()




graphics = """
points
lines
line_strip
triangles
triangle_strip
triangle_fan
quads
quad_strip
polygon
""".split()


primitives = """
vertices
color
""".split()

texts = """
text
text_scale
text_clip
""".split()

transforms = """
translate
rotate
flip
scale
""".split()

inputs = """
input_motion
input_key
input_click
""".split()

other = """
hotspot
hotspot_click
""".split()


sections = [
    ["SUMMON General Functions", commands],
    ["SUMMON Graphics", graphics],
    ["SUMMON Primitives", primitives],
    ["SUMMON Text constructs", texts],
    ["SUMMON Transforms", transforms],
    ["SUMMON Input specifications", inputs],
    ["SUMMON Miscellaneous", other]
]


out = sys.stdout

for name, lst in sections:
    print >>out, "%" * 70
    print >>out, "\subsection{%s}" % name
    print >>out
    for cmd in lst:
        doc = eval(cmd).func_doc
        if doc == None:
            continue
        args, usage = doc.split("\n")
        
        cmd = cmd.replace("_", "\\_")
        args = args.replace("_", "\\_")
        usage = usage.replace("_", "\\_")
        
        print >>out, "{\\tt %s%s }" % (cmd, args)
        print >>out 
        print >>out, "%s \\\\" % usage
        print >>out
        print >>out
