
#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

HANDLE hStdin;
DWORD fdwSaveOldMode;
DWORD wAttr;
DWORD wAttr2;
CONSOLE_CURSOR_INFO cci = { 100, FALSE };
CONSOLE_SCREEN_BUFFER_INFO cbi;
VOID ErrorExit(LPSTR);
VOID KeyEventProc(KEY_EVENT_RECORD, COORD &, HANDLE, string arr[], int size);
VOID MouseEventProc(MOUSE_EVENT_RECORD, COORD &, HANDLE, string arr[], int size);
string text[50];
COORD index = { 0, 0 };
int size;//array size


BOOL setxy(int x, int y)
{
	COORD c = { x, y };
	return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void DrawRect(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
	index.X = x + 1;
	index.Y = y + 1;
	setxy(x, y); cout << char(201);
	for (int i = 1; i < width; i++)cout << char(205);
	cout << char(187);
	setxy(x, height + y); cout << char(200);
	for (int i = 1; i < width; i++)cout << char(205);
	cout << char(188);
	for (int i = y + 1; i < height + y; i++)
	{
		setxy(x, i); cout << char(186);
		setxy(x + width, i); cout << char(186);
	}
	setxy(curPosX, curPosY);
}

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

	string text;
	string arr[] = { "[ ] test\n", "[ ] 1\n", "[ ] m2\n" };
	size = sizeof(arr) / sizeof(arr[0]);
	GetConsoleCursorInfo(h, &cci);
	cci.dwSize = 99;
	cci.bVisible = false;
	SetConsoleCursorInfo(h, &cci);

	GetConsoleScreenBufferInfo(h, &cbi);
	DWORD wAttr2 = cbi.wAttributes | 0x00;
	SetConsoleTextAttribute(h, wAttr2);

	for (int i = 0; i<size; i++)
	{
		cout << arr[i];
	}
	COORD p = { 1, 0 };
	SetConsoleCursorPosition(h, p);
	DetectRow(p, arr, h, size);

	wAttr = FOREGROUND_BLUE | FOREGROUND_INTENSITY; //blue foreground
	SetConsoleTextAttribute(h, wAttr);

	GetConsoleScreenBufferInfo(h, &cbi);
	wAttr2 = cbi.wAttributes | BACKGROUND_RED; //red background
	SetConsoleTextAttribute(h, wAttr2);

	DrawRect(20, 10, 15, 2);
	//x,y
	SetConsoleCursorPosition(h, p);

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

void Print(HANDLE h, string arr[])
{
	text->clear();
	for (int i = 0; i<size; i++)
	{
		if (arr[i][1] == 'X')
		{
			string s = arr[i].substr(4, arr[i].find('\n'));
			*text = text->substr(0, text->length());
			if (i>0)
			{
				text->erase(remove(text->begin(), text->end(), '\n'), text->end());
				const char * c = s.c_str();
				*text += s;

			}
			else
				*text += s;


		}
	}
	text[49] = '/0';
	SetConsoleCursorPosition(h, index);
	cout << *text;
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
			p.X = 0;
			SetConsoleCursorPosition(h, p);
			arr[p.Y][1] = 'X';
			cout << arr[p.Y];
			p.X++;
			Print(h, arr);
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
			SetConsoleCursorPosition(h, index);
			string clear;
			clear = " ";
			for (int i = 0; i<100; i++)
				cout << clear;
			wAttr = FOREGROUND_BLUE | FOREGROUND_INTENSITY; //blue foreground
			SetConsoleTextAttribute(h, wAttr);
			//SetConsoleTextAttribute(h,0xf0);
			GetConsoleScreenBufferInfo(h, &cbi);
			wAttr2 = cbi.wAttributes | BACKGROUND_RED; //red background
			SetConsoleTextAttribute(h, wAttr2);

			DrawRect(20, 10, 15, 2);
			SetConsoleTextAttribute(h, 0x0f);
			Print(h, arr);
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
					p.Y = mer.dwMousePosition.Y;
					p.X = 0;
					SetConsoleCursorPosition(h, p);
					arr[p.Y][1] = 'X';
					cout << arr[p.Y];
					p.X++;
					Print(h, arr);
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
