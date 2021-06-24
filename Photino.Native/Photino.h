#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <wil/com.h>
#include <WebView2.h>
typedef wchar_t* AutoString;
#else
// AutoString for macOS/Linux
typedef char* AutoString;
#endif

#ifdef __APPLE__
#include <Cocoa/Cocoa.h>
#include <WebKit/WebKit.h>
#endif

#ifdef __linux__
#include <gtk/gtk.h>
#endif

#include <map>
#include <string>
#include <vector>

struct Monitor
{
	struct MonitorRect
	{
		int x, y;
		int width, height;
	} monitor, work;
};

typedef void (*ACTION)();
typedef void (*WebMessageReceivedCallback)(AutoString message);
typedef void* (*WebResourceRequestedCallback)(AutoString url, int* outNumBytes, AutoString* outContentType);
typedef int (*GetAllMonitorsCallback)(const Monitor* monitor);
typedef void (*ResizedCallback)(int width, int height);
typedef void (*MovedCallback)(int x, int y);
typedef bool (*ClosingCallback)();

class Photino;
struct PhotinoInitParams
{
	AutoString StartString;
	AutoString StartUrl;
	AutoString Title;
	AutoString WindowIconFile;
	AutoString TemporaryFilesPath;

	Photino* ParentInstance;

	ClosingCallback* ClosingHandler;
	ResizedCallback* ResizedHandler;
	MovedCallback* MovedHandler;
	WebMessageReceivedCallback* WebMessageReceivedHandler;
	AutoString CustomSchemeNames[16];
	WebResourceRequestedCallback* CustomSchemeHandler;

	int Left;
	int Top;
	int Width;
	int Height;
	int Zoom;

	bool CenterOnInitialize;
	bool Chromeless;
	bool FullScreen;
	bool Maximized;
	bool Minimized;
	bool Resizable;
	bool Topmost;
	bool UseOsDefaultLocation;
	bool UseOsDefaultSize;

	int Size;
};

class Photino
{
private:
	WebMessageReceivedCallback _webMessageReceivedCallback;
	MovedCallback _movedCallback;
	ResizedCallback _resizedCallback;
	ClosingCallback _closingCallback;
	std::vector<wchar_t*> _customSchemeNames;
	WebResourceRequestedCallback _customSchemeCallback;

	wchar_t* _startUrl;
	wchar_t* _startString;
	wchar_t* _temporaryFilesPath;
	wchar_t* _windowTitle;

	int _zoom;
	Photino* _parent;
	void Show();
#ifdef _WIN32
	static HINSTANCE _hInstance;
	HWND _hWnd;
	wil::com_ptr<ICoreWebView2Environment> _webviewEnvironment;
	wil::com_ptr<ICoreWebView2> _webviewWindow;
	wil::com_ptr<ICoreWebView2Controller> _webviewController;
	bool EnsureWebViewIsInstalled();
	bool InstallWebView2();
	void AttachWebView();
#elif __linux__
	GtkWidget* _window;
	GtkWidget* _webview;
#elif __APPLE__
    // NSApplication *_app;
    NSWindow *_window;
    WKWebView *_webview;
	WKWebViewConfiguration *_webviewConfiguration;
    std::vector<Monitor *> GetMonitors();
#endif

public:
#ifdef _WIN32
	static void Register(HINSTANCE hInstance);
	HWND getHwnd();
	void RefitContent();
#elif __APPLE__
	static void Register();
#endif

	Photino(PhotinoInitParams* initParams);
	~Photino();

	void Center();
	void Close();
	void GetMaximized(bool* isMaximized);
	void GetMinimized(bool* isMinimized);
	void GetPosition(int* x, int* y);
	void GetResizable(bool* resizable);
	unsigned int GetScreenDpi();
	void GetSize(int* width, int* height);
	AutoString GetTitle();
	void GetTopmost(bool* topmost);
	void GetZoom(int* zoom);
	void Maximize();
	void Minimize();
	void NavigateToString(AutoString content);
	void NavigateToUrl(AutoString url);
	void Restore();
	void SendWebMessage(AutoString message);
	void SetIconFile(AutoString filename);
	void SetPosition(int x, int y);
	void SetResizable(bool resizable);
	void SetSize(int width, int height);
	void SetTitle(AutoString title);
	void SetTopmost(bool topmost);
	void SetZoom(int zoom);
	void ShowMessage(AutoString title, AutoString body, unsigned int type);
	void WaitForExit();

	//Callbacks
	void AddCustomSchemeName(AutoString scheme) { _customSchemeNames.push_back((wchar_t*)scheme); };
	void GetAllMonitors(GetAllMonitorsCallback callback);
	void SetClosingCallback(ClosingCallback callback) { _closingCallback = callback; }
	void SetMovedCallback(MovedCallback callback) { _movedCallback = callback; }
	void SetResizedCallback(ResizedCallback callback) { _resizedCallback = callback; }

	void Invoke(ACTION callback);
	bool InvokeClose() { if (_closingCallback) return _closingCallback(); else return false; }
	void InvokeMove(int x, int y) { if (_movedCallback) _movedCallback(x, y); }
	void InvokeResize(int width, int height) { if (_resizedCallback) _resizedCallback(width, height); }
};
