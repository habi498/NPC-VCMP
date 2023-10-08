if exist out-v141_xp rmdir out-v141_xp /S /Q
cmake . -B out-v141_xp -A Win32 -T v141_xp
IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%) ELSE (
cmake --build out-v141_xp --config Release
IF %ERRORLEVEL% NEQ 0 (Echo An error was found
exit /b %ERRORLEVEL%) ELSE (
echo Success
)
)