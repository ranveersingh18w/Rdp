@echo off
echo ==============================================
echo    Remote Desktop Streamer (Go Edition)
echo ==============================================
echo.
echo Connecting to DuckDNS server...
echo Server: ranveersingh.duckdns.org:8000
echo.

REM Run the Go streamer executable (DuckDNS URL is built-in)
streamer.exe

echo.
echo Stream ended.
pause
