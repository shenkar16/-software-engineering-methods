
#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

HANDLE hStdin;
DWORD fdwSaveOldMode;
int rowChanged = -1;
VOID ErrorExit(LPSTR);
VOID KeyEventProc(KEY_EVENT_RECORD, COORD &, HANDLE, string arr[], int size);
VOID MouseEventProc(MOUSE_EVENT_RECORD, COORD &, HANDLE, string arr[], int size);


void DetectRow(COORD p, string arr[], HANDLE h, int size)
{
	COORD curr = p;
	p.X = 0;
	p.Y = 0;
	SetConsoleTextAttribute(h, 0x0f);
	SetConsoleCursorPosition(h, p);

	for (int i = 0; i<size; i++)
	{
		if (i == curr.Y)
		{
			SetConsoleTextAttribute(h, 0xf0);
		}
		p.X = 0;
		SetConsoleCursorPosition(h, p);
		cout << arr[i];
		SetConsoleTextAttribute(h, 0x0f);
		p.Y++;
		SetConsoleCursorPosition(h, p);
	}
	p = curr;
	SetConsoleCursorPosition(h, p);

}

int main()
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD wAttr;
	string text;
	string arr[] = { "[ ] hello\n", "[ ] hello world\n", "[ ] hello selected world\n" };
	int size = sizeof(arr) / sizeof(arr[0]);
	CONSOLE_CURSOR_INFO cci = { 100, FALSE };
	GetConsoleCursorInfo(h, &cci);
	cci.dwSize = 99;
	cci.bVisible = false;
	SetConsoleCursorInfo(h, &cci);
	CONSOLE_SCREEN_BUFFER_INFO cbi;
	GetConsoleScreenBufferInfo(h, &cbi);
	DWORD wAttr2 = cbi.wAttributes | 0x00;
	SetConsoleTextAttribute(h, wAttr2);
	//DrawRect(1,1,10,1);
	for (int i = 0; i<size; i++)
	{
		cout << arr[i];
	}
	COORD p = { 1, 0 };
	SetConsoleCursorPosition(h, p);
	DetectRow(p, arr, h, size);
	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf[128];
	int counter = 0;
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE)
		ErrorExit("GetStdHandle");

	// Save the current input mode, to be restored on exit. 

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
		ErrorExit("GetConsoleMode");

	// Enable the window and mouse input events. 

	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(hStdin, fdwMode))
		ErrorExit("SetConsoleMode");

	// Loop to read and handle the next 100 input events. 

	while (1)
	{
		// Wait for the events. 

		if (!ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			128,         // size of read buffer 
			&cNumRead)) // number of records read 
			ErrorExit("ReadConsoleInput");

		// Dispatch the events to the appropriate handler. 

		for (i = 0; i < cNumRead; i++)
		{


			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT: // keyboard input 
				KeyEventProc(irInBuf[i].Event.KeyEvent, p, h, arr, size);
				break;

			case MOUSE_EVENT: // mouse input 
				MouseEventProc(irInBuf[i].Event.MouseEvent, p, h, arr, size);
				break;

			case FOCUS_EVENT:  // disregard focus events 

			case MENU_EVENT:   // disregard menu events 
				break;

			default:
				ErrorExit("Unknown event type");
				break;
			}

		}
	}

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);
	return 0;

}


VOID ErrorExit(LPSTR lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);

	ExitProcess(0);
}

VOID KeyEventProc(KEY_EVENT_RECORD ker, COORD &p, HANDLE h, string arr[], int size)
{

	if (ker.bKeyDown)
		if (ker.wVirtualKeyCode == VK_DOWN || ker.wVirtualKeyCode == VK_NUMPAD2){
			if (p.Y<2)
				p.Y++;
			SetConsoleCursorPosition(h, p);

		}

		else if (ker.wVirtualKeyCode == VK_UP || ker.wVirtualKeyCode == VK_NUMPAD8){
			if (p.Y>0)
				p.Y--;
			SetConsoleCursorPosition(h, p);
		}
		else if (ker.wVirtualKeyCode == 0x58 || ker.wVirtualKeyCode == VK_RETURN) {
			if (rowChanged != -1)
			{
				arr[rowChanged][1] = ' ';
			}
			p.X = 0;
			SetConsoleCursorPosition(h, p);
			arr[p.Y][1] = 'X';
			cout << arr[p.Y];
			rowChanged = p.Y;
			p.X++;
			SetConsoleCursorPosition(h, p);
		}
		else if (ker.wVirtualKeyCode == VK_BACK || ker.wVirtualKeyCode == VK_DELETE) {
			p.X = 0;
			SetConsoleCursorPosition(h, p);
			arr[p.Y][1] = ' ';
			cout << arr[p.Y];
			p.X++;
			SetConsoleCursorPosition(h, p);
			p.X++;
			SetConsoleCursorPosition(h, p);
			cout << "\b \b";
			p.X--;
			SetConsoleCursorPosition(h, p);

		}
		DetectRow(p, arr, h, size);
}

VOID MouseEventProc(MOUSE_EVENT_RECORD mer, COORD &p, HANDLE h, string arr[], int size)
{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif

	switch (mer.dwEventFlags)
	{
	case 0:

		if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			if (mer.dwMousePosition.Y<3)
			{
				if (mer.dwMousePosition.X == 1)
				{
					if (rowChanged != -1)
					{
						arr[rowChanged][1] = ' ';
					}
					p.Y = mer.dwMousePosition.Y;
					p.X = 0;
					SetConsoleCursorPosition(h, p);
					arr[p.Y][1] = 'X';
					cout << arr[p.Y];
					rowChanged = p.Y;
					p.X++;
					SetConsoleCursorPosition(h, p);
				}
				else
					p.Y = mer.dwMousePosition.Y;
				SetConsoleCursorPosition(h, p);

			}
		}

	}
	DetectRow(p, arr, h, size);
}
