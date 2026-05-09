# Remote Desktop Streamer - Usage Guide

## 🎯 Quick Start

### Prerequisites
- Windows 10/11
- Active internet connection
- Vercel signaling server running at: `wss://remote-sigma.vercel.app`

### Running the Application

**Option 1: Connect to Vercel Server (Default)**
```powershell
cd cpp-app\build\Release
.\streamer.exe
```

**Option 2: Custom Server**
```powershell
.\streamer.exe ws://your-custom-server:8000/ws/streamer
```

## 📋 Complete Setup Instructions

### 1. Build the Application (If Not Already Built)

```powershell
cd cpp-app
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The executable will be at: `cpp-app\build\Release\streamer.exe`

### 2. Run the Streamer

```powershell
# From the cpp-app directory
.\build\Release\streamer.exe
```

You should see:
```
==============================================
   Remote Desktop Screen Streamer Client
==============================================
Connecting to: wss://remote-sigma.vercel.app/ws/streamer
----------------------------------------------
Attempting connection...

[SUCCESS] Connected to signaling server!
Starting screen stream...
----------------------------------------------

[STREAMING] Your screen is now being streamed!
Open the web client to view the stream.
Press Enter to stop streaming...
```

### 3. View the Stream

Open your web browser and go to:
```
https://remote-sigma.vercel.app
```

Or open the HTML client locally from the `vercel-app` folder.

### 4. Stop Streaming

Press **Enter** in the console window where the streamer is running.

## 🔧 Configuration

### Change Screen Capture Quality

Edit `main.cpp` line 23 to adjust quality (0-100):
```cpp
ScreenStreamer(const std::string& serverUrl, int w = 1920, int h = 1080, int q = 75)
                                                                              // ↑↑ Change this
```
- **Lower** (30-50): Better performance, lower quality
- **Medium** (60-75): Balanced (default)
- **Higher** (80-100): Better quality, more bandwidth

### Change Frame Rate

Edit `main.cpp` line 58:
```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30 FPS
                                                            // ↑↑ Adjust timing
```
- **33ms** = ~30 FPS (default)
- **16ms** = ~60 FPS (smoother, more bandwidth)
- **66ms** = ~15 FPS (lower bandwidth)

## 🌐 Server Configuration

### Connecting to Different Servers

**Vercel Production:**
```powershell
.\streamer.exe wss://remote-sigma.vercel.app/ws/streamer
```

**Local Development:**
```powershell
.\streamer.exe ws://localhost:8000/ws/streamer
```

**Custom Domain:**
```powershell
.\streamer.exe wss://your-domain.com/ws/streamer
```

## 🐛 Troubleshooting

### "Failed to connect to server"

**Causes:**
1. Server is not running
2. Firewall blocking connection
3. Incorrect URL
4. No internet connection

**Solutions:**
- Check server status at: https://remote-sigma.vercel.app
- Temporarily disable firewall to test
- Verify URL is correct (wss:// for HTTPS, ws:// for HTTP)
- Test internet connection

### "Stream not visible in browser"

**Causes:**
1. WebSocket connection failed
2. Browser doesn't support WebRTC
3. CORS issues

**Solutions:**
- Check browser console (F12) for errors
- Use Chrome/Firefox/Edge (latest versions)
- Ensure you're accessing via HTTPS for Vercel

### Low Frame Rate or Lag

**Solutions:**
- Reduce quality setting (e.g., from 75 to 50)
- Increase sleep time (e.g., from 33ms to 50ms)
- Check network bandwidth
- Close other bandwidth-heavy applications

## 📊 Technical Details

### Architecture
```
┌─────────────────┐         ┌──────────────────┐         ┌─────────────────┐
│  C++ Streamer   │────────▶│  Vercel Server   │◀────────│  Web Browser    │
│  (Screen Cap)   │  WSS    │  (Signaling)     │  HTTPS  │  (Viewer)       │
└─────────────────┘         └──────────────────┘         └─────────────────┘
```

### Connection Flow
1. C++ app captures screen → JPEG encoding
2. Send binary frame via WebSocket
3. Vercel server relays to connected viewers
4. Browser receives and displays frame

### Features Implemented
✅ WebSocket client (WS/WSS support)
✅ Screen capture using GDI+
✅ JPEG compression
✅ Binary frame streaming
✅ ~30 FPS streaming
✅ Automatic reconnection handling
✅ URL parsing (ws:// and wss://)

### Network Requirements
- **Bandwidth:** ~1-5 Mbps (depends on quality/resolution)
- **Latency:** Works best with <100ms ping
- **Ports:** 443 (HTTPS/WSS) or 80 (HTTP/WS)

## 🚀 Advanced Usage

### Running as Background Service

Create a batch file `start_streamer.bat`:
```batch
@echo off
cd /d "%~dp0"
start /min .\streamer.exe wss://remote-sigma.vercel.app/ws/streamer
```

### Running on Startup

1. Press `Win + R`, type `shell:startup`
2. Copy `start_streamer.bat` to this folder
3. Application will start automatically on Windows login

### Multiple Screens

Currently captures primary screen. To modify for specific screen, edit `main.cpp`:
```cpp
// In captureScreen() method
HDC hdcScreen = GetDC(NULL);  // Primary screen

// For secondary screen (example):
// HDC hdcScreen = CreateDC("DISPLAY", "\\\\.\\DISPLAY2", NULL, NULL);
```

## 📝 Command Line Arguments

```powershell
# Basic usage
.\streamer.exe

# Custom server
.\streamer.exe [SERVER_URL]

# Examples:
.\streamer.exe wss://remote-sigma.vercel.app/ws/streamer
.\streamer.exe ws://192.168.1.100:8000/ws/streamer
.\streamer.exe wss://my-custom-domain.com/ws/streamer
```

## 🔐 Security Notes

- Uses **WSS** (WebSocket Secure) for encrypted communication
- Screen data is transmitted in real-time (not stored)
- No authentication currently implemented
- Recommended for trusted networks or add authentication layer

## 📞 Support

For issues or questions:
1. Check troubleshooting section above
2. Verify server is running: https://remote-sigma.vercel.app
3. Check browser console for WebSocket errors
4. Verify firewall/antivirus settings

## 🎉 Success Checklist

- [ ] Executable built successfully
- [ ] Server URL configured correctly
- [ ] Streamer connects without errors
- [ ] Web browser can access the viewer page
- [ ] Stream is visible in browser
- [ ] Frame rate is acceptable
- [ ] Can stop/start stream as needed

---

**Ready to stream!** 🖥️ → 📡 → 🌐
