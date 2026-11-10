@echo off
mkdir ..\build
cd build
cmake ..
set CMAKE_RESULT=%ERRORLEVEL%
if not %CMAKE_RESULT% == 0 pause
cmake --build .
set CMAKE_RESULT=%ERRORLEVEL%
if not %CMAKE_RESULT% == 0 pause
cd Debug