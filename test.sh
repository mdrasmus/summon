#!/bin/sh


export PYTHONPATH=lib

python <<EOF

import summon_core
from summon.core import color

LINES = 1064
GROUP = 1060
TRANSLATE = 1078

class Construct:
    def __init__(self, constructid, code):
        try:
            self.ptr = summon_core.make_construct(constructid, code)
        except:
            self.ptr = None
            raise

    def __del__(self):
        if self.ptr != None:
            summon_core.delete_construct(self.ptr)

class group (Construct):
    def __init__(self, *code):
        Construct.__init__(self, GROUP, code)

class lines (Construct):
    def __init__(self, *code):
        Construct.__init__(self, LINES, code)

class translate (Construct):
    def __init__(self, *code):
        Construct.__init__(self, TRANSLATE, code)


#c = Construct()
l = lines(color(1,1,1), 0, 0, 100, 100)
g = group(translate(10, 10, l))

EOF
