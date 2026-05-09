from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse, FileResponse
from fastapi.staticfiles import StaticFiles
import json
import logging
from pathlib import Path

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = FastAPI()

# CORS middleware for Vercel deployment
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Store active connections
connections = {
    "streamers": [],  # Go/C++ clients streaming
    "viewers": []     # Browser viewers
}

@app.get("/")
async def root():
    """Serve the main viewer page"""
    return FileResponse("index.html")

@app.get("/api/health")
async def health():
    """Health check endpoint with connection stats"""
    return {
        "status": "healthy",
        "streamers": len(connections["streamers"]),
        "viewers": len(connections["viewers"]),
        "version": "2.0-webrtc-ready"
    }

@app.websocket("/ws/streamer")
async def streamer_endpoint(websocket: WebSocket):
    """WebSocket endpoint for screen streamers (Go/C++ clients)"""
    await websocket.accept()
    connections["streamers"].append(websocket)
    logger.info(f"✓ Streamer connected. Total: {len(connections['streamers'])}")
    
    # Notify streamer of current viewer count
    await websocket.send_json({
        "type": "viewer_count",
        "count": len(connections["viewers"])
    })

    try:
        while True:
            # Receive data from streamer (can be binary frames or JSON signaling)
            message = await websocket.receive()
            
            # Check if it's binary data (JPEG frames from Go client)
            if "bytes" in message:
                data = message["bytes"]
                
                # Only forward frames if there are viewers
                if len(connections["viewers"]) > 0:
                    for viewer in connections["viewers"]:
                        try:
                            await viewer.send_bytes(data)
                        except Exception as e:
                            logger.error(f"Error sending frame to viewer: {e}")
                        
            # Check if it's text data (JSON signaling)
            elif "text" in message:
                try:
                    msg_data = json.loads(message["text"])
                    
                    # Forward WebRTC signaling to viewers
                    logger.info(f"Signaling message: {msg_data.get('type', 'unknown')}")
                    for viewer in connections["viewers"]:
                        try:
                            await viewer.send_json({"type": "signaling", "data": msg_data})
                        except Exception as e:
                            logger.error(f"Error forwarding signaling to viewer: {e}")
                except json.JSONDecodeError:
                    logger.error("Received invalid JSON")

    except WebSocketDisconnect:
        connections["streamers"].remove(websocket)
        logger.info(f"✗ Streamer disconnected. Total: {len(connections['streamers'])}")
    except Exception as e:
        logger.error(f"Streamer error: {e}")
        if websocket in connections["streamers"]:
            connections["streamers"].remove(websocket)

@app.websocket("/ws/viewer")
async def viewer_endpoint(websocket: WebSocket):
    """WebSocket endpoint for browser viewers"""
    await websocket.accept()
    connections["viewers"].append(websocket)
    logger.info(f"✓ Viewer connected. Total: {len(connections['viewers'])}")
    
    # Notify all streamers that a viewer connected
    for streamer in connections["streamers"]:
        try:
            await streamer.send_json({
                "type": "viewer_count",
                "count": len(connections["viewers"])
            })
        except Exception as e:
            logger.error(f"Error notifying streamer: {e}")

    try:
        while True:
            # Receive messages from viewer (control events, WebRTC signaling)
            try:
                data = await websocket.receive_text()
                message = json.loads(data)
                
                msg_type = message.get('type', 'unknown')
                
                # Handle mouse/keyboard control events
                if msg_type in ['mouse_move', 'mouse_click', 'mouse_scroll', 'keyboard']:
                    logger.info(f"Control event: {msg_type}")
                    # Forward control events to streamers
                    for streamer in connections["streamers"]:
                        try:
                            await streamer.send_json(message)
                        except Exception as e:
                            logger.error(f"Error forwarding control to streamer: {e}")
                else:
                    # Forward other signaling to streamers
                    logger.info(f"Viewer signaling: {msg_type}")
                    for streamer in connections["streamers"]:
                        try:
                            await streamer.send_json({"type": "signaling", "data": message})
                        except Exception as e:
                            logger.error(f"Error forwarding to streamer: {e}")
                        
            except json.JSONDecodeError:
                # Viewers typically don't send binary data
                pass

    except WebSocketDisconnect:
        connections["viewers"].remove(websocket)
        logger.info(f"✗ Viewer disconnected. Total: {len(connections['viewers'])}")
        
        # Notify all streamers that a viewer disconnected
        for streamer in connections["streamers"]:
            try:
                await streamer.send_json({
                    "type": "viewer_count",
                    "count": len(connections["viewers"])
                })
            except Exception as e:
                logger.error(f"Error notifying streamer: {e}")
                
    except Exception as e:
        logger.error(f"Viewer error: {e}")
        if websocket in connections["viewers"]:
            connections["viewers"].remove(websocket)

@app.get("/api/connections")
async def get_connections():
    """Get current connection status"""
    return {
        "streamers": {
            "count": len(connections["streamers"]),
            "active": len(connections["streamers"]) > 0
        },
        "viewers": {
            "count": len(connections["viewers"]),
            "active": len(connections["viewers"]) > 0
        }
    }
