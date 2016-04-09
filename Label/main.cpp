
#include <windows.h>
#include <stdio.h>
#include "Label.h"
#include <string>
#include <iostream>

HANDLE hStdin;
DWORD fdwSaveOldMode;

VOID ErrorExit(LPSTR);
VOID KeyEventProc(KEY_EVENT_RECORD);
VOID MouseEventProc(MOUSE_EVENT_RECORD);
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD);
Label_a::Label_a(string text, int h, int w)
{
	cont = text;
	c.X = w;
	c.Y = h;
	wAttr = FOREGROUND_GREEN | FOREGROUND_INTENSITY; //green foreground
	wAttr2 = 0; //black background
}



void BackToBlack(HANDLE &a, DWORD Attr = 7)  //change text color back to standard white
{
	SetConsoleTextAttribute(a, Attr);

}

void SetAndPrint(Label_a d, HANDLE h, CONSOLE_SCREEN_BUFFER_INFO cbi)
{
	SetConsoleCursorPosition(h, d.retC());
	SetConsoleTextAttribute(h, d.GetFColor());
	GetConsoleScreenBufferInfo(h, &cbi);
	d.SetBColor(cbi.wAttributes | d.GetBColor());
	SetConsoleTextAttribute(h, d.GetBColor());
	cout << d.GetCont() << "\n";
	BackToBlack(h);
}

int main()
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD wAttr;
	string text;
	int x, y;
	CONSOLE_SCREEN_BUFFER_INFO cbi = {};
	//DrawRect(1,1,10,1);
	while (1)
	{
		cout << "enter text:\n";
		cin >> text;
		cout << "enter x,y\n";
		cin >> x >> y;
		Label_a l(text, x, y);
		SetAndPrint(l, h, cbi);
	}
	return 0;
}
