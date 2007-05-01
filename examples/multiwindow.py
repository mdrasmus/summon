#!/usr/bin/python -i

from summon.core import *
import summon

wind1 = summon.Window("window 1")
wind1.add_group(group(lines(0, 0, 100,100)))

wind2 = summon.Window("window 2")
wind2.add_group(group(lines(0, 0, 100,100)))


def tie_windows(win1, win2, tiex=True, tiey=True):
    def tie(w1, w2):
        def update_view():
            # get window1 stats
            coords = w1.get_visible()
            size = w1.get_window_size()
            zoomx, zoomy = w1.get_zoom()

            # get window2 stats
            coords2 = w2.get_visible()
            coords3 = list(coords2)
            if tiex:
                coords3[0] = coords[0]
                coords3[2] = coords[2]
            if tiey:
                coords3[1] = coords[1]
                coords3[3] = coords[3]
            
            w2.set_visible(*coords3)
            size2 = w2.get_window_size()    
            zoomx2, zoomy2 = w2.get_zoom()

            # adjust window2
            w2.focus(size2[0] / 2.0, size2[1] / 2.0)
            
            if tiex:
                zoomratiox = zoomx2 / zoomx
            else:
                zoomratiox = 1.0
            
            if tiey:
                zoomratioy = zoomy2 / zoomy
            else:
                zoomratioy = 1.0
            
            w2.zoom(zoomratiox, zoomratioy)
        return update_view

    # add callbacks to window1
    win1.add_binding(input_motion("left", "down"), tie(win1, win2))
    win1.add_binding(input_motion("right", "down"), tie(win1, win2))
    win1.add_binding(input_motion("right", "down", "ctrl"), tie(win1, win2))
    win1.add_binding(input_motion("right", "down", "shift"), tie(win1, win2))

    # add callbacks to window2
    win2.add_binding(input_motion("left", "down"), tie(win2, win1))
    win2.add_binding(input_motion("right", "down"), tie(win2, win1))
    win2.add_binding(input_motion("right", "down", "ctrl"), tie(win2, win1))
    win2.add_binding(input_motion("right", "down", "shift"), tie(win2, win1))

tie_windows(wind1, wind2, tiex=False)
