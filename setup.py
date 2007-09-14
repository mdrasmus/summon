#!/usr/bin/env python
# 
# setup for SUMMON package
#
# use the following to install summon:
#   python setup.py install
#

from distutils.core import setup, Extension

SUMMON_VERSION = '1.8.2'

setup(
    name='summon',
    version=SUMMON_VERSION,
    description='A general 2D visualization prototyping module',
    long_description = """
        SUMMON is a python extension module that provides rapid prototyping of
        2D visualizations. By heavily relying on the python scripting language,
        SUMMON allows the user to rapidly prototype a custom visualization for
        their data, without the overhead of a designing a graphical user
        interface or recompiling native code. By simplifying the task of
        designing a visualization, users can spend more time on understanding
        their data.

        SUMMON was designed with several philosophies. First,
        recompilation should be avoided in order to speed up the development
        process. Second, design of graphical user interfaces should also be
        minimized. Designing a good interface takes planning and time to layout
        buttons, scrollbars, and dialog boxes. Yet a poor interface is very
        painful to work with. Even when one has a good interface, rarely can it
        be automated for batch mode. Instead, SUMMON relies on the python
        terminal for most interaction. This allows the users direct access to
        the underlining code, which is more expressive, and can be automated
        through scripting.

        Lastly, SUMMON is designed to be fast. Libraries already exist
        for accessing OpenGL in python. However, python is relatively slow for
        real-time interaction with large visualizations (trees with 100,000
        leaves, matrices with a million non-zeros, etc.). Therefore, all
        real-time interaction is handled with compiled native C++ code. Python
        is only executed in the construction and occasional interaction with the
        visualization. This arrangement provides the best of both worlds. 
    """,
    author='Matt Rasmussen',
    author_email='rasmus@mit.edu',
    url='http://people.csail.mit.edu/rasmus/summon/',
    download_url='http://compbio.mit.edu/pub/summon/summon-%s.tar.gz' % SUMMON_VERSION,
    
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
             "src/Summon/Element.cpp",
             "src/Summon/Graphic.cpp",
             "src/Summon/Hotspot.cpp",
             "src/Summon/Summon.cpp",
             "src/Summon/TextElement.cpp",
             "src/Summon/Transform.cpp",
             "src/Summon/TransformMatrix.cpp",
             "src/Summon/summonCommands.cpp",
             "src/Summon/SummonController.cpp",
             "src/Summon/SummonModel.cpp",
             "src/Summon/SummonView.cpp",
             "src/Summon/ZoomClamp.cpp"],
            include_dirs=["src/MVC",
                          "src/GlutMVC",
                          "src/Common",
                          "src/Summon"],
            libraries=["glut", "GL", "SDL"],
            undef_macros=["NDEBUG"])]
            # don't use -DNDEBUG (it breaks summon at runtime)
    )



"""

MAC OS X may need some of the following extra variables set

include_dirs=[
    "src/MVC",
    "src/GlutMVC",
    "src/Common",
    "src/Summon",
    "/sw/include/",
    "/usr/X11R6/include",
    "/Library/Frameworks/Python.framework/Versions/2.4/include/python2.4/"],
library_dirs=[
    "/sw/lib/",
    "/Library/Frameworks/Python.framework/Versions/2.4/lib",
    "/usr/X11R6/lib"],
define_macros=[("NOGLUTEXT", "1")]

"""
