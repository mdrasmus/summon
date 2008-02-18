@echo off

rem Builds windows install for summon using MinGW

rem basic windows path
set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem

rem set this to your python path
set PATH=%PATH%;C:\Python24

rem set this to your MinGW path
set PATH=%PATH%;C:\MinGW\bin

python setup-windows.py build --compiler=mingw32
python setup-windows.py bdist --format=wininst
