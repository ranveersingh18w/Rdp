@echo off
echo ==============================================
echo    Remote Desktop Server
echo ==============================================
echo.
echo Starting FastAPI server on port 8000...
echo Server will be accessible at:
echo   - Local: http://localhost:8000
echo   - Network: http://YOUR_LOCAL_IP:8000
echo   - Internet: http://ranveersingh.duckdns.org:8000
echo.
echo Press Ctrl+C to stop the server
echo ==============================================
echo.

cd vercel-app
uvicorn main:app --host 0.0.0.0 --port 8000 --reload

pause
