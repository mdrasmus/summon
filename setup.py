#!/usr/bin/env python
# 
# setup for SUMMON pckage
#
# use the following to install summon:
#   python setup.py install
#

from distutils.core import setup, Extension

setup(
    name='summon',
    version='1.6',
    description='SUMMON visualization prototyping module',
    author='Matt Rasmussen',
    author_email='rasmus@mit.edu',
    url='http://mit.edu/rasmus',
    
    packages=['summon'],
    package_dir = {'': 'lib'},
    py_modules=['summon_config'],
    scripts=['bin/summon',
             'bin/summatrix',
             'bin/sumtree'],
    ext_modules=[
        Extension(
            'summon_core', 
            ["src/Common/Matrix.cpp",
             "src/Common/Script.cpp",
             "src/Common/ScriptCommand.cpp",
             "src/GlutMVC/Glut2DController.cpp",
             "src/GlutMVC/Glut2DView.cpp",
             "src/GlutMVC/GlutController.cpp",
             "src/GlutMVC/GlutView.cpp",
             "src/GlutMVC/glut2DCommands.cpp",
             "src/GlutMVC/glutInputs.cpp",
             "src/MVC/Binding.cpp",
             "src/MVC/Command.cpp",
             "src/MVC/Input.cpp",
             "src/MVC/Model.cpp",
             "src/MVC/StringCommand.cpp",
             "src/MVC/common.cpp",
             "src/Summon/DrawController.cpp",
             "src/Summon/DrawModel.cpp",
             "src/Summon/DrawView.cpp",
             "src/Summon/Graphic.cpp",
             "src/Summon/GroupTable.cpp",
             "src/Summon/Summon.cpp",
             "src/Summon/Transform.cpp",
             "src/Summon/drawCommands.cpp"],
            include_dirs=["src/MVC",
                          "src/GlutMVC",
                          "src/Common",
                          "src/Summon"],
            libraries=["glut", "GL", "SDL"],
            undef_macros=["NDEBUG"])]
            # don't use -DNDEBUG (it breaks summon at runtime)
    )
