@echo off
echo ==============================================
echo    Testing Remote Desktop Connection
echo ==============================================
echo.

REM Test 1: Check if Vercel backend is running
echo [1/3] Testing Vercel backend health...
curl -s https://remote-sigma.vercel.app/api/health
echo.
echo.

REM Test 2: Check connection stats
echo [2/3] Checking active connections...
curl -s https://remote-sigma.vercel.app/api/connections
echo.
echo.

REM Test 3: Instructions
echo [3/3] Ready to test!
echo.
echo Next steps:
echo   1. Run: go-server\LAUNCH.bat (to start Go streamer)
echo   2. Open: https://remote-sigma.vercel.app (in browser)
echo   3. Watch the stream appear!
echo.
echo ==============================================
pause
