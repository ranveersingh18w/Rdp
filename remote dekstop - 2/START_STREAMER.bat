@echo off
echo ==============================================
echo    Remote Desktop Streamer
echo ==============================================
echo.
echo Starting screen streamer...
echo Connecting to: ws://ranveersingh.duckdns.org:8000
echo.
echo This will keep retrying if connection drops
echo Press Ctrl+C to stop
echo ==============================================
echo.

cd go-server

:retry
echo [%date% %time%] Starting streamer...
streamer.exe ws://ranveersingh.duckdns.org:8000/ws/streamer

echo.
echo [%date% %time%] Streamer stopped. Restarting in 5 seconds...
timeout /t 5 /nobreak >nul
goto retry
