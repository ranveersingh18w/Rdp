@echo off
echo ==============================================
echo    Remote Desktop - Complete Local Setup
echo ==============================================
echo.
echo This will start BOTH the server and streamer
echo.

REM Start server in new window
start "Backend Server" cmd /k "cd vercel-app && uvicorn main:app --host 0.0.0.0 --port 8000"

echo [1/2] Backend server starting in new window...
echo Waiting 5 seconds for server to initialize...
timeout /t 5 /nobreak >nul

echo.
echo [2/2] Starting Go streamer...
echo.

cd go-server
streamer.exe ws://localhost:8000/ws/streamer

echo.
echo ==============================================
echo Session ended.
pause
