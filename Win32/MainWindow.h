#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#define UNICODE
#define _UNICODE
#define _CRT_RAND_S

#include <Windows.h>
#include <tchar.h>

namespace MainWindow
{
	int Start(LPTSTR windowTitle = TEXT("")); // Open the main window and start the event loop
	void OnTimerEvent(void(*handler)()); // The callback is called periodically
	void ShowFps();
	int KeyPressed(int vkey); // Return whether a virtual key is pressed
	void UpdateScreen(); // Update the screen with the contents of the pixel buffer
	void PutPixel(int x, int y, int r, int g, int b); // Put a pixel in the buffer
	int ShowInfo(LPTSTR title, LPTSTR message); // Show an information message box
	int ShowWarning(LPTSTR title, LPTSTR message); // Show a warning message box
	int ShowError(LPTSTR title, LPTSTR message); // Show an error message box
	void Abort(LPTSTR title, LPTSTR message); // Abort program with an error message box
	void SetTitle(LPTSTR title); // Set the window title
}

#endif
