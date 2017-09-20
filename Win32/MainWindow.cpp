#include "MainWindow.h"
#include "Color.h"

#define CYCLE_DELAY 10
#define SCREEN_W	320
#define SCREEN_H	240
#define WINDOW_DEFAULT_W	640
#define WINDOW_DEFAULT_H	480

namespace MainWindow
{
	HINSTANCE HAppInstance = NULL;
	HWND HMainWindow = NULL;
	HDC HdcMainWindow = NULL;
	BITMAPINFO ScreenBitmapInfo;
	BYTE* ScreenBitmapData = NULL;
	int ClientAreaWidth = 0;
	int ClientAreaHeight = 0;
	UINT TimerID = 0;

	Color Screen[SCREEN_W * SCREEN_H];

	void(*OnTimerEventHandler)() = NULL;

	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void SetWindowSize(int width, int height);
	void DrawPixelLinear(int index, int r, int g, int b);
	void ShowFps();

	// FPS Counter
	DWORD dwFrames;
	DWORD dwCurrentTime;
	DWORD dwLastUpdateTime;
	DWORD dwElapsedTime;
	TCHAR szFPS[256];
	// End FPS Counter

	int Start(LPTSTR windowTitle)
	{
		// FPS Counter
		dwFrames = 0;
		dwCurrentTime = 0;
		dwLastUpdateTime = 0;
		dwElapsedTime = 0;
		szFPS[0] = '\0';
		// End FPS Counter

		HAppInstance = GetModuleHandle(NULL);

		/*INITCOMMONCONTROLSEX icce;
		icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icce.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&icce);*/

		WNDCLASSEX wcBasic = { 0 };
		wcBasic.lpszClassName = TEXT("wcMainWindow");
		wcBasic.cbSize = sizeof(WNDCLASSEX);
		wcBasic.hInstance = HAppInstance;
		wcBasic.hIcon = NULL;
		wcBasic.hIconSm = NULL;
		wcBasic.hCursor = NULL;
		wcBasic.lpszMenuName = NULL;
		wcBasic.cbClsExtra = 0;
		wcBasic.cbWndExtra = 0;
		wcBasic.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
		wcBasic.lpfnWndProc = MainWindowProc;
		wcBasic.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

		RegisterClassEx(&wcBasic);

		DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		DWORD dwExStyle = 0;

		ScreenBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		ScreenBitmapInfo.bmiHeader.biWidth = SCREEN_W;
		ScreenBitmapInfo.bmiHeader.biHeight = SCREEN_H;
		ScreenBitmapInfo.bmiHeader.biPlanes = 1;
		ScreenBitmapInfo.bmiHeader.biBitCount = 32;
		ScreenBitmapInfo.bmiHeader.biCompression = BI_RGB;
		ScreenBitmapInfo.bmiHeader.biSizeImage = 0;
		ScreenBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
		ScreenBitmapInfo.bmiHeader.biYPelsPerMeter = 0;
		ScreenBitmapInfo.bmiHeader.biClrUsed = 0;
		ScreenBitmapInfo.bmiHeader.biClrImportant = 0;

		HWND hMainWin = CreateWindowEx(dwExStyle, TEXT("wcMainWindow"), TEXT(""),
			dwStyle, 400, 200, WINDOW_DEFAULT_W, WINDOW_DEFAULT_H, NULL, NULL, HAppInstance, NULL);

		SetTitle(windowTitle);
		SetWindowSize(WINDOW_DEFAULT_W, WINDOW_DEFAULT_H);
		ShowWindow(hMainWin, TRUE);
		UpdateWindow(hMainWin);
		UpdateScreen();

		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.wParam;
	}

	void OnTimerEvent(void(*handler)())
	{
		OnTimerEventHandler = handler;
	}

	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
	{
		switch (uMsg)
		{
			case WM_CREATE:
			{
				HMainWindow = hwnd;
				TimerID = SetTimer(hwnd, 1, CYCLE_DELAY, NULL);
				break;
			}
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
			case WM_PAINT:
			{
				UpdateScreen();
				return DefWindowProc(hwnd, uMsg, wparam, lparam);
			}
			case WM_TIMER:
			{
				if (OnTimerEventHandler)
					OnTimerEventHandler();

				break;
			}
			default:
			{
				return DefWindowProc(hwnd, uMsg, wparam, lparam);
			}
		}

		return 0;
	}

	int KeyPressed(int vkey)
	{
		return GetAsyncKeyState(vkey) & (1 << 16);
	}

	void SetWindowSize(int width, int height)
	{
		ClientAreaWidth = width;
		ClientAreaHeight = height;

		RECT rcClient, rcWind;
		POINT ptDiff;
		GetClientRect(HMainWindow, &rcClient);
		GetWindowRect(HMainWindow, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		MoveWindow(HMainWindow, rcWind.left, rcWind.top, width + ptDiff.x, height + ptDiff.y, TRUE);
	}

	void UpdateScreen()
	{
		void* bmp = NULL;
		HBITMAP hbmp = CreateDIBSection(HdcMainWindow, &ScreenBitmapInfo, DIB_RGB_COLORS, &bmp, NULL, 0);
		ScreenBitmapData = (BYTE*)bmp;

		for (int i = 0; i < SCREEN_W * SCREEN_H; i++)
		{
			Color& pixel = Screen[i];
			DrawPixelLinear(i, pixel.R, pixel.G, pixel.B);
		}

		HdcMainWindow = GetDC(HMainWindow);
		//SetDIBitsToDevice(HdcMainWindow, 0, 0, SCREEN_W, SCREEN_H, 0, 0, 0, SCREEN_H, pixels, &ScreenBitmapInfo, DIB_PAL_COLORS);
		StretchDIBits(HdcMainWindow,
			0, 0, ClientAreaWidth, ClientAreaHeight,
			0, 0, SCREEN_W, SCREEN_H,
			bmp, &ScreenBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		DeleteObject(hbmp);
		ReleaseDC(HMainWindow, HdcMainWindow);
	}

	void DrawPixelLinear(int index, int r, int g, int b)
	{
		index <<= 2;

		ScreenBitmapData[index++] = b;
		ScreenBitmapData[index++] = g;
		ScreenBitmapData[index++] = r;
		ScreenBitmapData[index++] = 0;
	}

	void PutPixel(int x, int y, int r, int g, int b)
	{
		if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
			return;

		Screen[(SCREEN_H - y - 1) * SCREEN_W + x] = Color(r, g, b);
	}

	void ShowFps()
	{
		dwFrames++;
		dwCurrentTime = timeGetTime();
		dwElapsedTime = dwCurrentTime - dwLastUpdateTime;

		if (dwElapsedTime >= 1000)
		{
			wsprintf(szFPS, TEXT("FPS: %u"), (UINT)(dwFrames * 1000.0 / dwElapsedTime));
			dwFrames = 0;
			dwLastUpdateTime = dwCurrentTime;
		}

		SetWindowText(HMainWindow, szFPS);
	}

	int ShowError(LPTSTR title, LPTSTR message)
	{
		return MessageBox(HMainWindow, message, title, MB_ICONERROR);
	}

	int ShowWarning(LPTSTR title, LPTSTR message)
	{
		return MessageBox(HMainWindow, message, title, MB_ICONWARNING);
	}

	int ShowInfo(LPTSTR title, LPTSTR message)
	{
		return MessageBox(HMainWindow, message, title, MB_ICONINFORMATION);
	}

	void SetTitle(LPTSTR title)
	{
		SetWindowText(HMainWindow, title);
	}

	void Abort(LPTSTR title, LPTSTR message)
	{
		KillTimer(HMainWindow, TimerID);
		ShowError(title, message);
		PostQuitMessage(0);
	}
}
