#!/usr/bin/env python
# 
# setup for SUMMON package
#
# use the following to install summon:
#   python setup.py install
#

import os
from distutils.core import setup, Extension
import make_mac
from version import SUMMON_VERSION

setup(
    name='summon',
    version=SUMMON_VERSION,
    description='A general 2D visualization prototyping module',
    long_description = """
        SUMMON is a python extension module that provides rapid prototyping of
        2D visualizations.  By heavily relying on the python scripting language,
        SUMMON allows the user to rapidly prototype a custom visualization for
        their data,  without the overhead of designing a graphical user
        interface or recompiling  native code.  By simplifying the task of
        designing a visualization, users can  spend more time on understanding
        their data. 

        SUMMON is designed to be a fast interface for developing interactive
        visualizations (via scene graphs) for OpenGL. Although python libraries
        already exist for accessing OpenGL, python is relatively slow for
        real-time interaction with large visualizations (trees with 100,000
        leaves, sparse matrices with a million non-zeros, etc.). Therefore, with
        SUMMON all real-time interaction is handled with compiled native C++
        code (via extension module). Python is only executed in the construction
        and occasional interaction with the visualization. This arrangement
        provides the best of both worlds. 

        SUMMON was designed with several philosophies.  First, a scripting
        language  (Python) is used to speed up the development process and avoid
        overhead such as code  compilation.  Second, design of graphical user
        interfaces should be minimized.  Designing a good interface takes
        planning and time to layout buttons, scrollbars, and dialog boxes.  Yet
        a  poor interface is very painful to work with. Even when one has a good
        interface, rarely can it be automated for batch mode.  Instead, SUMMON
        relies on the Python  prompt for most interaction.  This allows the
        users direct access to  the underlying code, which is more expressive,
        and can be automated through scripting. 
    """,
    author='Matt Rasmussen',
    author_email='rasmus@mit.edu',
    url='http://people.csail.mit.edu/rasmus/summon/',
    download_url='http://people.csail.mit.edu/rasmus/summon/download/summon-%s.tar.gz' % SUMMON_VERSION,

    
    classifiers=[
          'Development Status :: 5 - Production/Stable',
          'Environment :: Console',
          'Environment :: Win32 (MS Windows)',
          'Environment :: X11 Applications',
          'Intended Audience :: Developers',
          'Intended Audience :: Education',
          'Intended Audience :: End Users/Desktop',
          'Intended Audience :: Science/Research',
          'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
          'Operating System :: MacOS :: MacOS X',
          'Operating System :: Microsoft :: Windows',
          'Operating System :: POSIX',
          'Programming Language :: Python',
          'Programming Language :: C++',
          'Topic :: Artistic Software',
          'Topic :: Education',
          'Topic :: Multimedia :: Graphics',
          'Topic :: Scientific/Engineering :: Visualization',
          ],
    
    package_dir = {'': 'lib'},
    packages=['summon'],
    py_modules=['summon_config'],
    scripts=['bin/summon',
             'bin/summatrix',
             'bin/sumtree',
             'bin/python-i'],
    ext_modules=[
        Extension(
            'summon_core', 
            ["src/Common/Script.cpp",
             "src/Common/common.cpp",
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
	     "src/SummonSceneGraph/summon_scene_graph.cpp",
             "src/SummonSceneGraph/elements.cpp",
             "src/SummonSceneGraph/Element.cpp",
             "src/SummonSceneGraph/Graphic.cpp",
             "src/SummonSceneGraph/Hotspot.cpp",
             "src/SummonSceneGraph/TextElement.cpp",
             "src/SummonSceneGraph/Transform.cpp",
             "src/SummonSceneGraph/TransformMatrix.cpp",
             "src/SummonSceneGraph/ZoomClamp.cpp",
	     "src/Summon/ScriptCommand.cpp",
             "src/Summon/Summon.cpp",             
             "src/Summon/summonCommands.cpp",
             "src/Summon/SummonController.cpp",
             "src/Summon/SummonModel.cpp",
             "src/Summon/SummonView.cpp"],
            include_dirs=["src/MVC",
                          "src/GlutMVC",
                          "src/Common",
                          "src/Summon",
                          "src/SummonSceneGraph",

                          "mac/include",
                          "/Developer/SDKs/MacOSX10.6.sdk/usr/X11/lib",
                          "/Developer/SDKs/MacOSX10.5.sdk/usr/X11/lib"],
                          #"/usr/local/include"],
                          #"/usr/X11R6/include"],
            libraries=["SDL", "glut"],
            library_dirs=["/Developer/SDKs/MacOSX10.6.sdk/usr/X11/lib",
                          "/Developer/SDKs/MacOSX10.5.sdk/usr/X11/lib",
                          "/opt/local/lib",
                          "/sw/lib"],
            define_macros=[("NOGLUTEXT", "1")],

            undef_macros=["NDEBUG"],

            extra_link_args=["-framework OpenGL -framework Cocoa"],

)],

            # don't use -DNDEBUG (it breaks summon at runtime)

    )


