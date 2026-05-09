@echo off
title Remote Desktop Streamer
echo ===============================================
echo    Remote Desktop Screen Streamer
echo ===============================================
echo.
echo Starting the screen streamer...
echo.
echo Default Server: wss://remote-sigma.vercel.app
echo.

cd /d "%~dp0"

if not exist "build\Release\streamer.exe" (
    echo [ERROR] streamer.exe not found!
    echo Please build the application first:
    echo    cd cpp-app
    echo    mkdir build
    echo    cd build
    echo    cmake ..
    echo    cmake --build . --config Release
    echo.
    pause
    exit /b 1
)

echo Launching streamer...
echo.
.\build\Release\streamer.exe

echo.
echo Streamer stopped.
pause
