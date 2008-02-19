"""

    SUMMON - common shapes
    
    This module provideds functions for building common shapes.

"""

from summon import *
import math



def box(x1, y1, x2, y2, fill=True):
    """
    Draws a rectangle (aligned with the x and y axis)
    
    x1, y1 -- one corner of rectangle
    x2, y2 -- the opposite corner of the rectangle
    fill   -- a bool indicating whether the rectangle is filled or stroked
    """

    if fill:
        return quads(x1, y1, x2, y1, x2, y2, x1, y2)
    else:
        return line_strip(x1, y1, x2, y1, x2, y2, x1, y2, x1, y1)


def round_box(x1, y1, x2, y2, radius, ndivs=10, fill=True):
    """
    Draws a round rectanged (aligned with the x and y axis)
    
    x1, y1 -- one corner of rectangle
    x2, y2 -- the opposite corner of the rectangle
    radius -- the radius of the rounded corner
              if radius is a list of 4 floats, it will be used to specify
              a radius for each corner (NW, NE, SE, SW)
    ndivs  -- number of divisions for each corner
    fill   -- a bool indicating whether the rectangle is filled or stroked    
    
    """

    # determine radii
    if hasattr(radius, "__iter__"):
        r1, r2, r3, r4 = radius
    else:
        r1 = r2 = r3 = r4 = radius

    # orient coords
    if x1 > x2:
        x1, x2 = x2, x1
    if y1 > y2:
        y1, y2 = y2, y1
    
    pts = []
    pts.extend(arc_path(x2-r2, y2-r2, .5 * math.pi, 0.0, r2, ndivs))
    pts.extend(arc_path(x2-r3, y1+r3, 0.0, -.5 * math.pi,  r3, ndivs))
    pts.extend(arc_path(x1+r4, y1+r4, -.5 * math.pi, -math.pi,  r4, ndivs))
    pts.extend(arc_path(x1+r1, y2-r1, -math.pi, -1.5*math.pi,  r1, ndivs))
    
    if fill:
        return polygon(*pts)
    else:
        pts.append(x2-r2)
        pts.append(y2)
        return line_strip(*pts)
    
    return group()


def arc_path(x, y, angle1, angle2, radius, ndivs=10):
    """Returns the points for a round corner
    
    x      -- x-coordinate of arc center
    y      -- y-coordinate of arc center
    angle1 -- starting angle of arc
    angle2 -- ending angle of arc
    radius -- radius of arc
    ndivs  -- number of divisions for arc
    """
    
    pts = []
    i =  angle1
    curve = float(angle2 - angle1)
    
    if angle2 < angle1:
        while i >= angle2:
            pts.append(x + radius * math.cos(i))
            pts.append(y + radius * math.sin(i))
            i += curve / ndivs
    else:
        while i <= angle2:
            pts.append(x + radius * math.cos(i))
            pts.append(y + radius * math.sin(i))
            i += (.25 * math.pi / ndivs)
    return pts


def regular_polygon(x, y, nsides, radius, fill=True, rotate=0.0):
    """
    Draws a regular 'nsides'-sided polygon with size 'radius'
    
    x, y   -- a point that defines the center of the polygon
    fill   -- a bool indicating whether polygon is filled or stroked
    rotate -- the number of degrees to rotate the polygon counter-clockwise
    """
    
    rotate *= 3.14159 / 180.0
    i = rotate
    pts = []
    while i < rotate + 2 * 3.14159:
        pts.append(x + radius * math.cos(i))
        pts.append(y + radius * math.sin(i))
        i += (2 * 3.14159 / nsides)
    
    if fill:
        return polygon(*pts)
    else:
        pts.append(radius)
        pts.append(0)
        return line_strip(*pts)



def arrow(headx, heady, tailx, taily, *tail, **options):
    """
    Draws an arrow
    
    headx -- x-coordinate of the arrow head
    heady -- y-coordinate of the arrow head
    tailx -- x-coordinate of the arrow tail
    taily -- y-coordinate of the arrow tail
    *tail -- additional tail points
    style     -- arrow style. choose from: 'solid' 
                 (only one implemented currently)
    head_size -- size of arrow head (default: 10)
    offset    -- place arrow head an a different location (x, y)
                 (default: (headx, heady))
    """

    # get options
    style = options.get("style", "solid")
    head_size = options.get("head_size", 10)
    offset = options.get("offset", (headx, heady))
    
    if style == "solid":
        return group(line_strip(headx, heady, tailx, taily, *tail),
                     zoom_clamp(
                        arrow_head(offset[0], offset[1], tailx, taily, 
                                   head_size),
                        minx=1, miny=1, maxx=1, maxy=1, 
                        origin=(headx, heady),
                        axis=(tailx, taily)))
    else:
        raise Exception("unknown arrow style '%s'" % style)    


def arrow_head(headx, heady, tailx, taily, size=10):
    """
    Draws a triangular arrow head
    
    headx -- x-coordinate of the arrow head
    heady -- y-coordinate of the arrow head
    tailx -- x-coordinate of the arrow tail
    taily -- y-coordinate of the arrow tail    
    size  -- size of arrow head
    """

    # compute arrow head vector (unit vector)
    vecx = headx - tailx
    vecy = heady - taily
    length = math.sqrt(vecx*vecx + vecy*vecy)
    vecx *= size / length
    vecy *= size / length
    
    return polygon(headx, heady,
                   headx - vecx - vecy, heady - vecy + vecx,
                   headx - vecx + vecy, heady - vecy - vecx)


def message_bubble(x, y, width, height, contents=None,
                   tail=(10,30), tail_width=20, 
                   tail_attach=.3, radius=10,
                   bubble_color=color(1, 1, 1),
                   border_color=color(0, 0, 0)):
    """
    Draws a message bubble
    
    x            -- x-coordinate of target
    y            -- y-coordinate of target
    width        -- width of bubble
    height       -- height of bubble
    contents     -- content of bubble (drawing elements)
    tail         -- tail direction (x, y)
    tail_width   -- width of tail when it attaches to bubble
    tail_attach  -- percentage of bubble to the left of the tail
    radius       -- radius of bubble's rounded corners
    bubble_color -- color of message bubble
    """
    
    # compute bubble position
    bx = x+tail[0]-tail_attach*width
    by = y+tail[1]
    
    # create default content if none given
    if contents == None:
        contents = group()
    
    
    return zoom_clamp(
        # tail
        bubble_color,
        triangles(x, y, 
                  x+tail[0], y+tail[1],
                  x+tail[0]+tail_width, y+tail[1]),
        
        # bubble
        round_box(bx, by, bx+width, by+height, radius=radius),
        
        
        # border
        border_color,
        line_strip(*
            arc_path(bx+radius, by+radius, 1.5*math.pi, math.pi, radius) + \
            arc_path(bx+radius, by+height-radius, math.pi, .5*math.pi, radius) + \
            arc_path(bx+width-radius, by+height-radius, .5*math.pi, 0.0, radius) + \
            arc_path(bx+width-radius, by+radius, 0.0, -.5*math.pi, radius) + \
            [x+tail[0]+tail_width, y+tail[1],
             x, y,
             x+tail[0], y+tail[1],
             bx+radius, by]),
        
        # bubble contents
        translate(bx, by, contents),
        minx=1, miny=1, maxx=1, maxy=1, link=True, origin=(x, y))



def draggable(region, aGroup):
    def onDrag(event, x, y):
        if event == "drag_start":
            mouse[:] = [trans[0] + x, trans[1] + y]
        
        elif event == "drag_stop":
            mouse[:] = []

            
        elif event == "drag":
            if len(mouse) == 0:
                return
            click = [trans[0] + x, trans[1] + y]

            vx = click[0] - mouse[0]
            vy = click[1] - mouse[1]
            mouse[:] = click
            
            trans[0] += vx
            trans[1] += vy
                
            vis.set(trans[0], trans[1])
    
    mouse = []
    trans = [0, 0]
    vis = translate(0, 0, group(aGroup,
                    hotspot_region("drag", region, onDrag)))
    return vis

