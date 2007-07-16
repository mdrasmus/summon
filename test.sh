#!/bin/sh


export PYTHONPATH=lib

python <<EOF

import summon_core

class Construct:
    def __init__(self):
        self.ptr = summon_core.make_construct()

    def __del__(self):
        summon_core.delete_construct(self.ptr)


c = Construct()

EOF
