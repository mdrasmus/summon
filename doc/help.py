from summon import *
import sys


commands = """
add_group
get_visible                      
assign_model        get_window                    
call_proc           get_window_size                    
  get_windows         quit                timer_call
clear_binding       group               redraw_call         trans
clear_groups        home                remove_group        
close_window                     replace_group       
               hotspot_click                     
del_model                                   
                               set_antialias       version
                            set_bgcolor         
focus               insert_group        set_binding         zoom
get_bgcolor                   set_model           zoomx
get_group                          set_visible         zoomy
get_model           new_model           set_window          
get_models          new_window          set_window_size     
get_root_id                       show_group
clear_all_bindings
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


out = file(sys.argv[2], "w")

for name, lst in sections:
    print >>out, "%" * 70
    print >>out, "\subsection{ %s }" % name
    print >>out
    for cmd in lst:
        args, usage = eval(cmd).func_doc.split("\n")
        
        cmd = cmd.replace("_", "\\_")
        args = args.replace("_", "\\_")
        usage = usage.replace("_", "\\_")
        
        print >>out, "{\\tt %s%s }" % (cmd, args)
        print >>out 
        print >>out, "%s \\\\" % usage
        print >>out
        print >>out
