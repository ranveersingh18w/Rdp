@echo off
cls
color 0B
title Remote Desktop Streamer - v1.0

:: ASCII Art Banner
echo.
echo  ========================================================
echo  ███████╗████████╗██████╗ ███████╗ █████╗ ███╗   ███╗
echo  ██╔════╝╚══██╔══╝██╔══██╗██╔════╝██╔══██╗████╗ ████║
echo  ███████╗   ██║   ██████╔╝█████╗  ███████║██╔████╔██║
echo  ╚════██║   ██║   ██╔══██╗██╔══╝  ██╔══██║██║╚██╔╝██║
echo  ███████║   ██║   ██║  ██║███████╗██║  ██║██║ ╚═╝ ██║
echo  ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝
echo  ========================================================
echo           Remote Desktop Screen Streamer v1.0
echo  ========================================================
echo.
echo  Server: wss://remote-sigma.vercel.app
echo  Status: Initializing...
echo.
echo  --------------------------------------------------------
echo.

cd /d "%~dp0"

:: Check if executable exists
if not exist "build\Release\streamer.exe" (
    color 0C
    echo  [ERROR] Executable not found!
    echo.
    echo  Expected location: build\Release\streamer.exe
    echo.
    echo  Please build the application first:
    echo    1. cd cpp-app\build
    echo    2. cmake --build . --config Release
    echo.
    pause
    exit /b 1
)

echo  [OK] Executable found
echo  [OK] Starting connection...
echo.
echo  --------------------------------------------------------
echo.

:: Run the streamer
.\build\Release\streamer.exe

:: After streaming stops
echo.
echo  --------------------------------------------------------
echo  Stream ended. Thank you for using Remote Desktop Streamer!
echo  --------------------------------------------------------
echo.
pause
