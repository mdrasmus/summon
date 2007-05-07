@echo off
rem run commands in a proper windows environment.

rem set path environment variable
set Path=C:\Program Files\Microsoft Visual Studio 8\Common7\IDE;C:\Program Files\Microsoft Visual Studio 8\VC\BIN;C:\Program Files\Microsoft Visual Studio 8\Common7\Tools;C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\bin;C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;C:\Program Files\Microsoft Visual Studio 8\VC\VCPackages;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\PROGRA~1\ULTRAE~1;C:\Program Files\ATI Technologies\ATI Control Panel;C:\Program Files\MIT\Kerberos\bin;C:\Program Files\Common Files\Adobe\AGL;C:\Documents and Settings\rasmus\My Documents\download\gp400win32\gnuplot\bin\;C:\Program Files\QuickTime\QTSystem\;C:\Program Files\OpenAFS\Common;C:\Program Files\OpenAFS\Client\Program;C:\Python24

rem execute given command
%*
