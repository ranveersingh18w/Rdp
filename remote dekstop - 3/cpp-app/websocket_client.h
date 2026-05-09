#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#pragma comment(lib, "winhttp.lib")

class WebSocketClient {
private:
    std::string url;
    std::string host;
    std::string path;
    int port;
    bool useSSL;
    bool connected;

    HINTERNET hSession;
    HINTERNET hConnect;
    HINTERNET hWebSocket;

    bool parseURL(const std::string& serverUrl) {
        std::string urlCopy = serverUrl;

        if (urlCopy.find("wss://") == 0) {
            useSSL = true;
            urlCopy = urlCopy.substr(6);
            port = 443;
        } else if (urlCopy.find("ws://") == 0) {
            useSSL = false;
            urlCopy = urlCopy.substr(5);
            port = 80;
        } else {
            useSSL = false;
            port = 80;
        }

        size_t pathPos = urlCopy.find('/');
        if (pathPos != std::string::npos) {
            host = urlCopy.substr(0, pathPos);
            path = urlCopy.substr(pathPos);
        } else {
            host = urlCopy;
            path = "/";
        }

        size_t portPos = host.find(':');
        if (portPos != std::string::npos) {
            port = std::stoi(host.substr(portPos + 1));
            host = host.substr(0, portPos);
        }

        return true;
    }

public:
    WebSocketClient(const std::string& serverUrl) 
        : url(serverUrl), useSSL(false), connected(false),
          hSession(NULL), hConnect(NULL), hWebSocket(NULL) {
        parseURL(serverUrl);
    }

    ~WebSocketClient() {
        disconnect();
    }

    bool connect() {
        // Initialize WinHTTP session
        hSession = WinHttpOpen(
            L"RemoteDesktopStreamer/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0
        );

        if (!hSession) {
            std::cerr << "WinHttpOpen failed: " << GetLastError() << std::endl;
            return false;
        }

        // Convert host to wide string
        int hostLen = MultiByteToWideChar(CP_UTF8, 0, host.c_str(), -1, NULL, 0);
        wchar_t* wHost = new wchar_t[hostLen];
        MultiByteToWideChar(CP_UTF8, 0, host.c_str(), -1, wHost, hostLen);

        // Connect to server
        hConnect = WinHttpConnect(hSession, wHost, port, 0);
        delete[] wHost;

        if (!hConnect) {
            std::cerr << "WinHttpConnect failed: " << GetLastError() << std::endl;
            WinHttpCloseHandle(hSession);
            return false;
        }

        // Convert path to wide string
        int pathLen = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, NULL, 0);
        wchar_t* wPath = new wchar_t[pathLen];
        MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, wPath, pathLen);

        // Open WebSocket request
        DWORD flags = useSSL ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            wPath,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            flags
        );
        delete[] wPath;

        if (!hRequest) {
            std::cerr << "WinHttpOpenRequest failed: " << GetLastError() << std::endl;
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        // Upgrade to WebSocket
        if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, NULL, 0)) {
            std::cerr << "Failed to set WebSocket upgrade option: " << GetLastError() << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        // Send request
        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
            std::cerr << "WinHttpSendRequest failed: " << GetLastError() << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        // Receive response
        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            std::cerr << "WinHttpReceiveResponse failed: " << GetLastError() << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        // Complete WebSocket handshake
        hWebSocket = WinHttpWebSocketCompleteUpgrade(hRequest, 0);
        WinHttpCloseHandle(hRequest);

        if (!hWebSocket) {
            std::cerr << "WinHttpWebSocketCompleteUpgrade failed: " << GetLastError() << std::endl;
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        connected = true;
        return true;
    }

    void disconnect() {
        if (hWebSocket) {
            WinHttpWebSocketClose(hWebSocket, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, NULL, 0);
            WinHttpCloseHandle(hWebSocket);
            hWebSocket = NULL;
        }
        if (hConnect) {
            WinHttpCloseHandle(hConnect);
            hConnect = NULL;
        }
        if (hSession) {
            WinHttpCloseHandle(hSession);
            hSession = NULL;
        }
        connected = false;
    }

    bool sendBinary(const std::vector<BYTE>& data) {
        if (!connected || data.empty()) return false;

        DWORD error = WinHttpWebSocketSend(
            hWebSocket,
            WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE,
            (PVOID)data.data(),
            data.size()
        );

        if (error != ERROR_SUCCESS) {
            std::cerr << "WinHttpWebSocketSend failed: " << error << std::endl;
            return false;
        }

        return true;
    }

    bool sendText(const std::string& text) {
        if (!connected || text.empty()) return false;

        DWORD error = WinHttpWebSocketSend(
            hWebSocket,
            WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE,
            (PVOID)text.c_str(),
            text.length()
        );

        if (error != ERROR_SUCCESS) {
            std::cerr << "WinHttpWebSocketSend failed: " << error << std::endl;
            return false;
        }

        return true;
    }
};

#endif // WEBSOCKET_CLIENT_H
