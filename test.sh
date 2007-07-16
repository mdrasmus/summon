#!/bin/sh


export PYTHONPATH=lib

python <<EOF

import summon_core

LINES = 1064

class Construct:
    def __init__(self, constructid, code):
        self.ptr = summon_core.make_construct(constructid, code)

    def __del__(self):
        summon_core.delete_construct(self.ptr)

class lines (Construct):
    def __init__(self, *code):
        Construct.__init__(self, LINES, code)


#c = Construct()
l = lines(0, 0, 100, 100)


EOF
