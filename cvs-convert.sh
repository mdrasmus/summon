#!/usr/bin/env python

import os

x="""src/Common/Color.h
src/Common/Matrix.cpp
src/Common/Matrix.h
src/Common/RectTree.h
src/Common/Window.h
src/GlutMVC/Glut2DController.cpp
src/GlutMVC/Glut2DController.h
src/GlutMVC/Makefile
src/GlutMVC/glutInputs.cpp
src/GlutMVC/glutInputs.h
src/MVC/Command.cpp
src/MVC/Command.h
src/MVC/Controller.h
src/MVC/Factory.h
src/MVC/Input.cpp
src/MVC/Input.h
src/MVC/Makefile
src/MVC/Model.cpp
src/MVC/Model.h
src/MVC/View.h
src/MVC/common.h
src/Summon/Group.h
src/Summon/GroupTable.cpp
src/Summon/Hotspot.h
src/notes/design.dia
src/notes/ideas.txt
""".split()

os.system("mkdir -p src/notes")

for f in x:
    print f
    os.system("cp ../personal/summon/%s %s" % (f, f))
    os.system("git add %s" % f)
