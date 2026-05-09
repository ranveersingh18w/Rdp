@echo off
echo ==============================================
echo    Remote Desktop Streamer (LOCAL)
echo ==============================================
echo.
echo Starting screen streamer...
echo Connecting to: ws://localhost:8000
echo.
echo NOTE: This connects to LOCAL server
echo For remote streaming, use a different PC
echo.
echo This will keep retrying if connection drops
echo Press Ctrl+C to stop
echo ==============================================
echo.

cd go-server

:retry
echo [%date% %time%] Starting streamer...
streamer.exe ws://localhost:8000/ws/streamer

echo.
echo [%date% %time%] Streamer stopped. Restarting in 5 seconds...
timeout /t 5 /nobreak >nul
goto retry
