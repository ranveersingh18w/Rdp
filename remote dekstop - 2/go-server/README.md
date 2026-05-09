# Go Remote Desktop Streamer

A Windows executable written in Go that captures your screen and streams it to the Vercel backend via WebSocket.

## 🚀 Quick Start

### Build the Executable

```powershell
# Install dependencies
go get github.com/pion/webrtc/v3
go get github.com/gorilla/websocket
go get github.com/kbinani/screenshot

# Build the executable
go build -o streamer.exe main.go
```

### Run the Streamer

```powershell
# Connect to default Vercel server
.\streamer.exe

# Or specify custom server
.\streamer.exe wss://your-app.vercel.app/ws/streamer
.\streamer.exe ws://localhost:8000/ws/streamer
```

## 📋 Features

- **Screen Capture**: Captures primary display at 30 FPS
- **JPEG Compression**: 75% quality for optimal bandwidth
- **WebSocket Streaming**: Sends frames to Vercel backend
- **Auto-reconnect**: Handles connection drops gracefully
- **Cross-platform**: Works on Windows, macOS, Linux

## 🔧 Configuration

Edit `main.go` to customize:

```go
const (
    defaultServerURL = "wss://remote-sigma.vercel.app/ws/streamer"
    frameRate        = 30 // FPS
    jpegQuality      = 75 // 1-100
)
```

## 📦 Dependencies

- **github.com/pion/webrtc/v3** - WebRTC implementation
- **github.com/gorilla/websocket** - WebSocket client
- **github.com/kbinani/screenshot** - Cross-platform screen capture

## 🎯 Usage Flow

1. **Build** the Go executable
2. **Deploy** Vercel backend (if not already deployed)
3. **Run** the streamer: `.\streamer.exe`
4. **Open** browser viewer at your Vercel URL
5. **View** the live screen stream

## 🌐 Vercel Integration

The Go client connects to your Vercel backend at:
- **Production**: `wss://your-app.vercel.app/ws/streamer`
- **Local Dev**: `ws://localhost:8000/ws/streamer`

The Vercel backend forwards frames to browser viewers connected to `/ws/viewer`.

## 🐛 Troubleshooting

### Build Errors

```powershell
# Clear module cache
go clean -modcache

# Re-download dependencies
go mod tidy
go get -u ./...
```

### Connection Issues

- Verify Vercel backend is deployed and running
- Check firewall allows outbound WebSocket connections
- Use `wss://` for HTTPS/Vercel, `ws://` for local
- Test with: `curl https://your-app.vercel.app/api/health`

### Performance Issues

- Reduce `frameRate` (e.g., 15 FPS)
- Lower `jpegQuality` (e.g., 50)
- Check network bandwidth
- Monitor CPU usage

## 📊 Performance

Typical resource usage:
- **CPU**: 5-15% (depends on resolution)
- **Memory**: ~50 MB
- **Network**: ~2-5 Mbps (1080p @ 30 FPS, 75% quality)

## 🔒 Security

- Use WSS (WebSocket Secure) for production
- Consider adding authentication tokens
- Restrict Vercel CORS to trusted domains
- Monitor for unauthorized connections

## 📄 License

MIT License
