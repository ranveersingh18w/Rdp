# C++ Screen Streamer Application

A Windows C++ application that captures your screen and streams it via WebSocket to a signaling server (Vercel).

## 🚀 Quick Start

### Run the Application

```powershell
cd build\Release
.\streamer.exe
```

**Default Configuration:**
- Connects to: `wss://remote-sigma.vercel.app/ws/streamer`
- Captures: Primary screen at ~30 FPS
- Quality: 75% JPEG compression

### View the Stream

Open your browser: **https://remote-sigma.vercel.app**

## 📋 Requirements

- **Windows OS** (Windows 10/11)
- **CMake** 3.10 or higher
- **Visual Studio** 2019 or higher (with C++ Desktop Development)
- **Windows SDK** (for GDI+)
- **Internet Connection** (for Vercel server)

## 🏗️ Building the Application

### Method 1: Using Command Line

```powershell
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build (Release mode for best performance)
cmake --build . --config Release
```

The executable will be created at: `build\Release\streamer.exe`

### Method 2: Using CMake GUI

1. Open CMake GUI
2. Set source directory to this folder
3. Set build directory to `build`
4. Click "Configure" and select your Visual Studio version
5. Click "Generate"
6. Click "Open Project" to open in Visual Studio
7. Build the solution (Release mode recommended)

## 🚀 Running the Application

### Basic Usage (Vercel Server)

```powershell
.\build\Release\streamer.exe
```

This connects to `wss://remote-sigma.vercel.app/ws/streamer` by default.

### Connect to Custom Server

```powershell
# Local development server
.\streamer.exe ws://localhost:8000/ws/streamer

# Custom domain
.\streamer.exe wss://your-domain.com/ws/streamer
.\streamer.exe ws://your-server-url:port
```

### Connect to Vercel Deployment

```powershell
.\streamer.exe wss://your-app.vercel.app
```

**Note:** Use `wss://` (WebSocket Secure) for HTTPS/Vercel deployments.

## 📁 Files

- **main.cpp** - Main application with screen capture logic
- **websocket_client.h** - WebSocket client implementation
- **CMakeLists.txt** - CMake build configuration

## ⚙️ How It Works

1. **Screen Capture**: Uses Windows GDI+ to capture the screen
2. **Compression**: Encodes frames as JPEG (quality: 75%)
3. **Streaming**: Sends frames via WebSocket at ~30 FPS
4. **Connection**: Connects to `/ws/streamer` endpoint

## 🔧 Configuration

Edit `main.cpp` to change:

- **Resolution**: Default is full screen
- **Quality**: JPEG quality (1-100), default is 75
- **FPS**: Frame delay in milliseconds, default is 33ms (~30 FPS)

```cpp
ScreenStreamer streamer(serverUrl, 1920, 1080, 75);  // width, height, quality
```

## 🐛 Troubleshooting

### CMake not found
Download and install from: https://cmake.org/download/

### Build errors
- Ensure Visual Studio C++ Desktop Development is installed
- Check Windows SDK is installed
- Try cleaning the build directory and rebuilding

### Connection fails
- Verify the server is running
- Check firewall settings
- Ensure correct WebSocket URL (ws:// or wss://)

## 📝 Usage Flow

1. **Build** the application using CMake
2. **Start** the Vercel signaling server
3. **Run** the streamer executable
4. **Open** the web viewer and connect
5. **Press Enter** in the streamer console to stop

## 🔒 Security Notes

- This application captures your entire screen
- Only connect to trusted servers
- Use WSS (secure WebSocket) for production
- Consider adding authentication for production use

## 📄 License

MIT License
