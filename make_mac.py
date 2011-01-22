#!/usr/bin/env python

import os

if not os.path.exists("mac/include"): os.makedirs("mac/include")
if not os.path.exists("mac/lib"): os.makedirs("mac/lib")
if not os.path.exists("mac/include/GL"):
    os.symlink("/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers",
               "mac/include/GL")
