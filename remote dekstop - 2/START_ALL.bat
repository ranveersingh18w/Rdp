@echo off
echo ==============================================
echo    Remote Desktop - ALL IN ONE
echo ==============================================
echo.
echo Starting server and streamer together...
echo.
echo Server: http://localhost:8000
echo Viewers can access: http://ranveersingh.duckdns.org:8000
echo.
echo Press Ctrl+C to stop everything
echo ==============================================
echo.

REM Start server in background
start "Remote Desktop Server" cmd /c "cd vercel-app && uvicorn main:app --host 0.0.0.0 --port 8000"

echo Waiting 3 seconds for server to start...
timeout /t 3 /nobreak >nul

REM Start streamer in foreground with auto-reconnect
cd go-server

:retry
echo.
echo [%date% %time%] Starting streamer...
streamer.exe ws://127.0.0.1:8000/ws/streamer

echo.
echo [%date% %time%] Streamer stopped. Restarting in 5 seconds...
timeout /t 5 /nobreak >nul
goto retry
