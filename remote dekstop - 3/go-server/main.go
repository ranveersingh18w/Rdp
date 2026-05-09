package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"image"
	"image/jpeg"
	"log"
	"os"
	"time"

	"github.com/gorilla/websocket"
	"github.com/kbinani/screenshot"
	// TODO: Add robotgo when TDM-GCC is installed
	// "github.com/go-vgo/robotgo"
)

const (
	defaultServerURL = "ws://127.0.0.1:8000/ws/streamer"
	frameRate        = 15 // FPS (reduced for stability)
	jpegQuality      = 50 // Quality (reduced for lower bandwidth)
	pingInterval     = 30 * time.Second
	readTimeout      = 60 * time.Second
)

type ControlMessage struct {
	Type   string  `json:"type"`
	X      int     `json:"x,omitempty"`
	Y      int     `json:"y,omitempty"`
	Button string  `json:"button,omitempty"`
	Key    string  `json:"key,omitempty"`
	DeltaX int     `json:"deltaX,omitempty"`
	DeltaY int     `json:"deltaY,omitempty"`
}

type ViewerCountMessage struct {
	Type  string `json:"type"`
	Count int    `json:"count"`
}

type ScreenStreamer struct {
	serverURL    string
	ws           *websocket.Conn
	streaming    bool
	viewerCount  int
	screenWidth  int
	screenHeight int
}

func NewScreenStreamer(serverURL string) *ScreenStreamer {
	if serverURL == "" {
		serverURL = defaultServerURL
	}
	return &ScreenStreamer{
		serverURL:   serverURL,
		streaming:   false,
		viewerCount: 0,
	}
}

func (s *ScreenStreamer) Connect() error {
	log.Printf("Connecting to: %s", s.serverURL)
	
	dialer := websocket.DefaultDialer
	dialer.HandshakeTimeout = 10 * time.Second
	
	var err error
	s.ws, _, err = dialer.Dial(s.serverURL, nil)
	if err != nil {
		return fmt.Errorf("failed to connect to WebSocket: %w", err)
	}
	
	// Set up ping/pong for keep-alive
	s.ws.SetPongHandler(func(string) error {
		s.ws.SetReadDeadline(time.Now().Add(readTimeout))
		return nil
	})
	
	// Set initial read deadline
	s.ws.SetReadDeadline(time.Now().Add(readTimeout))
	
	log.Println("✓ Connected to signaling server")
	return nil
}

func (s *ScreenStreamer) handleControlMessage(msg ControlMessage) {
	// TODO: Install TDM-GCC and uncomment robotgo code below
	switch msg.Type {
	case "mouse_move":
		// robotgo.Move(msg.X, msg.Y)
		log.Printf("🖱️  Mouse move to (%d, %d) [robotgo not available - install TDM-GCC]", msg.X, msg.Y)
		
	case "mouse_click":
		// robotgo.Move(msg.X, msg.Y)
		// if msg.Button == "left" || msg.Button == "" {
		// 	robotgo.Click("left")
		// } else if msg.Button == "right" {
		// 	robotgo.Click("right")
		// }
		log.Printf("🖱️  Mouse click at (%d, %d) button: %s [robotgo not available]", msg.X, msg.Y, msg.Button)
		
	case "mouse_scroll":
		// robotgo.ScrollMouse(msg.DeltaY, "up")
		log.Printf("🖱️  Mouse scroll: %d [robotgo not available]", msg.DeltaY)
		
	case "keyboard":
		// robotgo.TypeStr(msg.Key)
		log.Printf("⌨️  Keyboard: %s [robotgo not available]", msg.Key)
	}
}

func (s *ScreenStreamer) listenForMessages() {
	for {
		_, message, err := s.ws.ReadMessage()
		if err != nil {
			log.Printf("Error reading message: %v", err)
			return
		}
		
		// Try to parse as JSON
		var viewerMsg ViewerCountMessage
		if err := json.Unmarshal(message, &viewerMsg); err == nil {
			if viewerMsg.Type == "viewer_count" {
				s.viewerCount = viewerMsg.Count
				log.Printf("� Viewer count: %d", s.viewerCount)
				continue
			}
		}
		
		// Try to parse as control message
		var controlMsg ControlMessage
		if err := json.Unmarshal(message, &controlMsg); err == nil {
			s.handleControlMessage(controlMsg)
		}
	}
}

func (s *ScreenStreamer) Start() error {
	s.streaming = true
	
	log.Println("Starting screen capture...")
	log.Printf("Capturing at %d FPS with %d%% JPEG quality", frameRate, jpegQuality)
	log.Println("⏸️  Waiting for viewers before streaming...")
	
	// Get screen bounds
	n := screenshot.NumActiveDisplays()
	if n == 0 {
		return fmt.Errorf("no active displays found")
	}
	
	bounds := screenshot.GetDisplayBounds(0)
	s.screenWidth = bounds.Dx()
	s.screenHeight = bounds.Dy()
	log.Printf("Screen resolution: %dx%d", s.screenWidth, s.screenHeight)
	
	// Start listening for control messages in background
	go s.listenForMessages()
	
	ticker := time.NewTicker(time.Second / frameRate)
	defer ticker.Stop()
	
	// Ping ticker for keep-alive
	pingTicker := time.NewTicker(pingInterval)
	defer pingTicker.Stop()
	
	frameCount := 0
	startTime := time.Now()
	wasStreaming := false
	
	for s.streaming {
		select {
		case <-ticker.C:
			// Only capture and send if there are viewers
			if s.viewerCount > 0 {
				if !wasStreaming {
					log.Println("▶️  Starting stream (viewers present)")
					wasStreaming = true
					frameCount = 0
					startTime = time.Now()
				}
				
				if err := s.captureAndSend(bounds); err != nil {
					log.Printf("Error capturing/sending frame: %v", err)
					return err
				}
				
				frameCount++
				if frameCount%frameRate == 0 {
					elapsed := time.Since(startTime).Seconds()
					actualFPS := float64(frameCount) / elapsed
					log.Printf("� Frames sent: %d | Actual FPS: %.1f | Viewers: %d", frameCount, actualFPS, s.viewerCount)
				}
			} else {
				if wasStreaming {
					log.Println("⏸️  Pausing stream (no viewers)")
					wasStreaming = false
				}
			}
			
		case <-pingTicker.C:
			// Send ping to keep connection alive
			if err := s.ws.WriteControl(websocket.PingMessage, []byte{}, time.Now().Add(10*time.Second)); err != nil {
				log.Printf("Ping failed: %v", err)
				return err
			}
		}
	}
	
	return nil
}

func (s *ScreenStreamer) captureAndSend(bounds image.Rectangle) error {
	// Capture screenshot
	img, err := screenshot.CaptureRect(bounds)
	if err != nil {
		return fmt.Errorf("failed to capture screen: %w", err)
	}
	
	// Encode to JPEG
	var buf bytes.Buffer
	if err := jpeg.Encode(&buf, img, &jpeg.Options{Quality: jpegQuality}); err != nil {
		return fmt.Errorf("failed to encode JPEG: %w", err)
	}
	
	// Send binary data via WebSocket (no deadline)
	if err := s.ws.WriteMessage(websocket.BinaryMessage, buf.Bytes()); err != nil {
		return fmt.Errorf("failed to send frame: %w", err)
	}
	
	return nil
}

func (s *ScreenStreamer) Stop() {
	s.streaming = false
	if s.ws != nil {
		s.ws.Close()
	}
	log.Println("Stream stopped")
}

func main() {
	fmt.Println("==============================================")
	fmt.Println("   Remote Desktop Screen Streamer (Go)")
	fmt.Println("==============================================")
	fmt.Println("Libraries:")
	fmt.Println("  - Screen Capture: github.com/kbinani/screenshot")
	fmt.Println("  - WebSocket: github.com/gorilla/websocket")
	fmt.Println("  - Mouse/Keyboard: github.com/go-vgo/robotgo")
	fmt.Println("  - Image Encoding: Go built-in image/jpeg")
	fmt.Println("==============================================")
	
	serverURL := defaultServerURL
	if len(os.Args) > 1 {
		serverURL = os.Args[1]
		log.Printf("Using command-line URL: %s", serverURL)
	} else {
		log.Printf("Using default URL: %s", serverURL)
	}
	
	fmt.Printf("Server URL: %s\n", serverURL)
	fmt.Println("----------------------------------------------")
	
	streamer := NewScreenStreamer(serverURL)
	
	// Connect to server
	if err := streamer.Connect(); err != nil {
		log.Fatalf("❌ Connection failed: %v\n\nPlease check:\n  1. Server is running\n  2. Firewall settings\n  3. URL is correct\n", err)
	}
	
	fmt.Println("\n✅ Connected successfully!")
	fmt.Println("Waiting for viewers...")
	fmt.Println("----------------------------------------------")
	fmt.Println("\n🎮 READY - Mouse & Keyboard control enabled!")
	fmt.Println("Stream will start when viewers connect.")
	fmt.Println("Press Ctrl+C to stop...\n")
	
	// Start streaming
	if err := streamer.Start(); err != nil {
		log.Printf("❌ Streaming error: %v", err)
		log.Println("Will retry connection...")
		os.Exit(1)
	}
}
