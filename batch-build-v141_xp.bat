::This will build 3 items- npcclient, plugin, rel004
@echo off 
if not exist out-v141xp (
mkdir out-v141xp
)
:: First build 32-bit (npcclient, npc04rel32, )

:: Generate 
cmake . -T v141_xp -A Win32 -B out-v141xp/Win32

IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%) ELSE (

:: Build it
cmake --build out-v141xp/Win32 --config Release

IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%)


IF %ERRORLEVEL% NEQ 0 (
echo Error occured while copying
exit /b %ERRORLEVEL%
)


IF %ERRORLEVEL% NEQ 0 (
echo Error occured while copying
exit /b %ERRORLEVEL%
)


)

:: Second build 64-bit( npc04rel64 )
:: Generate
cmake . -T v141_xp -A x64 -B out-v141xp/x64

IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%) ELSE (

::Build
cmake --build out-v141xp/x64 --config Release

IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%)


IF %ERRORLEVEL% NEQ 0 (
echo Error occured while copying
exit /b %ERRORLEVEL%
)

)

::Now build REL004 npcclient_r004.exe

:: Generate REL004
cmake . -T v141_xp -A Win32 -B out-v141xp/Win32-B -DREL004:INTEGER=1

IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%) ELSE (

:: Build it
cmake --build out-v141xp/Win32-B --config Release

IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%)

IF %ERRORLEVEL% NEQ 0 (
echo Error occured while copying
exit /b %ERRORLEVEL%
)


)
echo Success
