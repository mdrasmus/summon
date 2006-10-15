from summon import *
import summonlib



win = summonlib.defaultWindow()


vis = [
    lines(0,0, 100,200),
    lines(0,0, 100,100)
]

win.add_group(group(* vis))

