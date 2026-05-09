#include "websocket_client.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class ScreenStreamer {
private:
    WebSocketClient* wsClient;
    bool streaming;
    int width;
    int height;
    int quality;

public:
    ScreenStreamer(const std::string& serverUrl, int w = 1920, int h = 1080, int q = 75) 
        : streaming(false), width(w), height(h), quality(q) {
        wsClient = new WebSocketClient(serverUrl);
    }

    ~ScreenStreamer() {
        stop();
        delete wsClient;
    }

    bool connect() {
        return wsClient->connect();
    }

    void start() {
        streaming = true;
        std::thread(&ScreenStreamer::streamLoop, this).detach();
    }

    void stop() {
        streaming = false;
    }

private:
    void streamLoop() {
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        while (streaming) {
            auto frame = captureScreen();
            if (!frame.empty()) {
                wsClient->sendBinary(frame);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30 FPS
        }

        GdiplusShutdown(gdiplusToken);
    }

    std::vector<BYTE> captureScreen() {
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
        SelectObject(hdcMem, hBitmap);
        
        BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);
        
        Bitmap* bitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
        
        // Encode to JPEG
        std::vector<BYTE> buffer;
        IStream* stream = NULL;
        CreateStreamOnHGlobal(NULL, TRUE, &stream);
        
        CLSID jpegClsid;
        GetEncoderClsid(L"image/jpeg", &jpegClsid);
        
        EncoderParameters encoderParams;
        encoderParams.Count = 1;
        encoderParams.Parameter[0].Guid = EncoderQuality;
        encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
        encoderParams.Parameter[0].NumberOfValues = 1;
        encoderParams.Parameter[0].Value = &quality;
        
        bitmap->Save(stream, &jpegClsid, &encoderParams);
        
        STATSTG statstg;
        stream->Stat(&statstg, STATFLAG_DEFAULT);
        ULONG size = statstg.cbSize.LowPart;
        
        buffer.resize(size);
        LARGE_INTEGER li = {0};
        stream->Seek(li, STREAM_SEEK_SET, NULL);
        stream->Read(buffer.data(), size, NULL);
        
        stream->Release();
        delete bitmap;
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        
        return buffer;
    }

    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
        UINT num = 0, size = 0;
        GetImageEncodersSize(&num, &size);
        if (size == 0) return -1;
        
        ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        GetImageEncoders(num, size, pImageCodecInfo);
        
        for (UINT j = 0; j < num; ++j) {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;
            }
        }
        free(pImageCodecInfo);
        return -1;
    }
};

int main(int argc, char* argv[]) {
    std::string serverUrl = "wss://remote-sigma.vercel.app/ws/streamer";

    if (argc > 1) {
        serverUrl = argv[1];
    }

    std::cout << "==============================================\n";
    std::cout << "   Remote Desktop Screen Streamer Client\n";
    std::cout << "==============================================\n";
    std::cout << "Connecting to: " << serverUrl << "\n";
    std::cout << "----------------------------------------------\n";

    ScreenStreamer streamer(serverUrl);

    std::cout << "Attempting connection...\n";
    if (!streamer.connect()) {
        std::cerr << "\n[ERROR] Failed to connect to server!\n";
        std::cerr << "Please check:\n";
        std::cerr << "  1. Server is running at: " << serverUrl << "\n";
        std::cerr << "  2. Firewall is not blocking the connection\n";
        std::cerr << "  3. URL is correct (wss:// for secure, ws:// for local)\n\n";
        std::cout << "Press Enter to exit...\n";
        std::cin.get();
        return 1;
    }

    std::cout << "\n[SUCCESS] Connected to signaling server!\n";
    std::cout << "Starting screen stream...\n";
    std::cout << "----------------------------------------------\n";
    streamer.start();

    std::cout << "\n[STREAMING] Your screen is now being streamed!\n";
    std::cout << "Open the web client to view the stream.\n";
    std::cout << "Press Enter to stop streaming...\n\n";
    std::cin.get();

    std::cout << "Stopping stream...\n";
    streamer.stop();
    std::cout << "[STOPPED] Stream ended successfully.\n";
    std::cout << "==============================================\n";

    return 0;
}
