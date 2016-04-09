#include <iostream>
using namespace std;

class textField {
	
	HANDLE h;
	DWORD wAttr;
	DWORD wAttr2;
	CONSOLE_CURSOR_INFO cci;
	CONSOLE_SCREEN_BUFFER_INFO cbi;

	int _rectangleCoordX;
	int _rectangleCoordY;
	COORD p;

	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf[128];
	int counter;

	string* text;
	int index;
	int _width;

	VOID KeyEventProc(KEY_EVENT_RECORD ker, COORD &p, HANDLE h);
	VOID MouseEventProc(MOUSE_EVENT_RECORD mer, COORD &p, HANDLE h);
	VOID ErrorExit(LPSTR lpszMessage);

public:
	textField(int rectangleCoordX, int rectangleCoordY, int width);
	COORD retC(){ return p; }
	void SetBColor(DWORD a){ wAttr2 = a; };
	DWORD GetFColor(){ return wAttr; };
	DWORD GetBColor(){ return wAttr2; };
};