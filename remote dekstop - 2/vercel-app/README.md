# Remote Desktop Backend - Render Deployment

FastAPI WebSocket server for remote desktop streaming. Deploy this to Render for production use.

## 🚀 Quick Deploy to Render

### Method 1: Via Render Dashboard (Recommended)

1. **Create GitHub Repository** (if not already done)
   ```bash
   cd remote-desktop-backend
   git init
   git add .
   git commit -m "Initial commit - Remote Desktop Backend"
   git remote add origin https://github.com/YOUR_USERNAME/remote-desktop-backend.git
   git push -u origin main
   ```

2. **Deploy to Render**
   - Go to [Render Dashboard](https://dashboard.render.com/)
   - Click "New +" → "Web Service"
   - Connect your GitHub repository
   - Configure:
     - **Name**: `remote-desktop-backend`
     - **Region**: Choose closest to you
     - **Branch**: `main`
     - **Build Command**: `pip install -r requirements.txt`
     - **Start Command**: `uvicorn main:app --host 0.0.0.0 --port $PORT`
   - Click "Create Web Service"

3. **Get Your URL**
   - After deployment: `https://remote-desktop-backend.onrender.com`

### Method 2: Deploy Button

[![Deploy to Render](https://render.com/images/deploy-to-render-button.svg)](https://render.com/deploy)

## 📁 What's Included

```
remote-desktop-backend/
├── main.py              # FastAPI WebSocket server
├── index.html           # Web viewer UI
├── requirements.txt     # Python dependencies
├── Procfile            # Render process file
├── runtime.txt         # Python version
└── README.md           # This file
```

## � Configuration

### Environment Variables (Optional)

In Render Dashboard → Environment:

```
ALLOWED_ORIGINS=https://yourdomain.com
MAX_CONNECTIONS=10
```

### Custom Domain

1. Go to service settings in Render
2. Add custom domain
3. Update DNS records
4. Free SSL certificate included

## 🌐 API Endpoints

- `GET /` - Web viewer
- `GET /api/health` - Health check
- `GET /api/connections` - Connection stats
- `WS /ws/streamer` - For streaming clients
- `WS /ws/viewer` - For browser viewers

## 🎮 Usage

### Connect Go Streamer

```bash
.\streamer.exe wss://remote-desktop-backend.onrender.com/ws/streamer
```

### View in Browser

```
https://remote-desktop-backend.onrender.com
```

## 📊 Features

- ✅ Full WebSocket support (no timeouts)
- ✅ Real-time connection status
- ✅ Multi-viewer support
- ✅ Live statistics (FPS, bandwidth, uptime)
- ✅ Auto-reconnect
- ✅ Beautiful modern UI

## � Troubleshooting

### Service Won't Start

Check Render logs for errors:
- Verify Python version (3.11)
- Check all dependencies installed
- Ensure port binding uses `$PORT`

### WebSocket Connection Fails

- Use `wss://` (not `ws://`)
- Verify service is running
- Check firewall settings

## 📄 License

MIT License
