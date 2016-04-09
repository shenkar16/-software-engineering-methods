#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "textField.h"
using namespace std;

HANDLE hStdin;
DWORD fdwSaveOldMode;

BOOL setxy(int x, int y)
{
	COORD c = { x, y };
	return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

/*
*This method draws the rectangle.
*Usage:
*DrawRect(1,1,10,10);
*or
*DrawRect(1,1,10,10,20,5);
*/
void  DrawRect(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
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



textField::textField(int rectangleCoordX, int rectangleCoordY, int width)
{
	_rectangleCoordX = rectangleCoordX;
	_rectangleCoordY = rectangleCoordY;
	index = rectangleCoordX + 1;
	text = new string[width-1];
	_width = width;

	//access to out buffer of console
	 h = GetStdHandle(STD_OUTPUT_HANDLE);

	 //the curser coords postiion in the rectangle
	 p = { rectangleCoordX + 1, rectangleCoordY + 1 };

	 //The percentage of the character cell that is filled by the cursor. This value is between 1 and 100
	cci = { 100, FALSE };
	GetConsoleCursorInfo(h, &cci);
	cci.dwSize = 99;
	cci.bVisible = true;

	//Sets the size and visibility of the cursor for the specified console screen buffer.
	SetConsoleCursorInfo(h, &cci);
	GetConsoleScreenBufferInfo(h, &cbi);

	wAttr = FOREGROUND_BLUE | FOREGROUND_INTENSITY; //blue foreground
	SetConsoleTextAttribute(h, wAttr);

	GetConsoleScreenBufferInfo(h, &cbi);
	wAttr2 = cbi.wAttributes | BACKGROUND_RED; //red background
	SetConsoleTextAttribute(h, wAttr2);

	//draw rectangle height = 2
	DrawRect(rectangleCoordX, rectangleCoordY, width, 2);
	//the curser coords postiion in the rectangle
	p = { rectangleCoordX + 1, rectangleCoordY + 1 };

	SetConsoleCursorPosition(h, p);
	
	counter = 0;
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
				KeyEventProc(irInBuf[i].Event.KeyEvent, p, h);
				break;

			case MOUSE_EVENT: // mouse input 
				MouseEventProc(irInBuf[i].Event.MouseEvent, p, h);
				break;
				/*
				case FOCUS_EVENT:  // disregard focus events

				case MENU_EVENT:   // disregard menu events
				break;

				default:
				ErrorExit("Unknown event type");
				break;
				*/
			}

		}
	}

	// Restore input mode on exit.
	SetConsoleMode(hStdin, fdwSaveOldMode);

}

VOID textField::ErrorExit(LPSTR lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);

	ExitProcess(0);
}

VOID textField::KeyEventProc(KEY_EVENT_RECORD ker, COORD &p, HANDLE h)
{
	char letter;

	if (ker.bKeyDown == TRUE)
	{

		//initialize the border of the text field
		if (p.X > _rectangleCoordX && p.X < (_rectangleCoordX + _width) && p.Y == (_rectangleCoordY+1) ){

			//move right with arrow key 
			if (ker.wVirtualKeyCode == VK_RIGHT || ker.wVirtualKeyCode == VK_RWIN){
				//for the right border
				if (p.X != ((_rectangleCoordX + _width)-1))
					p.X++;

			}
			//move left with arrow key 
			else if (ker.wVirtualKeyCode == VK_LEFT || ker.wVirtualKeyCode == VK_LWIN){
				//for the left border
				if (p.X != (_rectangleCoordX+1))
					p.X--;
			}
			//if press delete key or backspace key
			else if (ker.wVirtualKeyCode == VK_BACK || ker.wVirtualKeyCode == VK_DELETE){
				char space = ' ';
				text[p.X - index] = space;
				cout << text[p.X - index];
				//if we in the end of string we dont want move left on the text field border
				if (p.X > (_rectangleCoordX + 1))
					p.X--;
			}
			else{
				letter = ker.uChar.AsciiChar;
				text[p.X - index] = letter;
				cout << text[p.X - index];
				//if we in the end of string we dont want move right on the text field border
				if (p.X != ((_rectangleCoordX + _width) - 1))
					p.X++;
			}
		}
	}
	SetConsoleCursorPosition(h, p);
}

VOID textField::MouseEventProc(MOUSE_EVENT_RECORD mer, COORD &p, HANDLE h)
{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif

	switch (mer.dwEventFlags)
	{
	case 0:

		if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			//initialize the border of the text field
			if (mer.dwMousePosition.X > _rectangleCoordX && mer.dwMousePosition.X < (_rectangleCoordX + _width) && mer.dwMousePosition.Y == (_rectangleCoordY+1))
			{
				p.X = mer.dwMousePosition.X;
				SetConsoleCursorPosition(h, p);
			}
		}

	}
	
}

int main()
{
	//rectangle COORD X,Y,WIDTH
	textField model(12, 5, 40);

	return 0;
	system("pasue");

}